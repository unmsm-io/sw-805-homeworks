#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>

#define N 10 // Maze size
#define EXIT_X 9
#define EXIT_Y 9
#define MAX_STACK 10000

int maze[N][N] = {
    {0, 1, 0, 0, 0, 1, 1, 0, 1, 0},
    {0, 1, 0, 1, 0, 0, 0, 1, 1, 0},
    {0, 1, 0, 1, 1, 1, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 0, 1, 1, 1, 0, 1},
    {0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
    {1, 1, 0, 1, 1, 1, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

// Moves: right, down, left, up
int moves[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

// Stack for iterative DFS
typedef struct
{
  int x, y;
} Point;
Point stack[MAX_STACK];
int top = -1;

void push(int x, int y)
{
  if (top < MAX_STACK - 1)
  {
    top++;
    stack[top].x = x;
    stack[top].y = y;
  }
}

int pop(int *x, int *y)
{
  if (top >= 0)
  {
    *x = stack[top].x;
    *y = stack[top].y;
    top--;
    return 1;
  }
  return 0;
}

int global_visited[N][N]; // Tracks which process visited each cell (-1 = unvisited)
int found_exit = 0;

// Add these global variables after the other globals
double start_time; // Track start time

void draw_maze(int rank, int maze_solved)
{
  if (rank != 0)
    return; // Only rank 0 draws
  clear();

  // Calculate elapsed time
  double current_time = MPI_Wtime();
  double elapsed = current_time - start_time;

  // Draw title box
  attron(COLOR_PAIR(6) | A_BOLD);
  mvprintw(0, 0, "+------------------------------------------------+");
  mvprintw(1, 0, "|            Parallel Maze Path Finder            |");
  mvprintw(2, 0, "+------------------------------------------------+");
  attroff(COLOR_PAIR(6) | A_BOLD);

  // Draw maze frame
  attron(COLOR_PAIR(7) | A_BOLD);
  mvprintw(4, 0, "+");
  for (int j = 0; j < N * 3; j++)
    mvprintw(4, j + 1, "-");
  mvprintw(4, N * 3 + 1, "+");

  // Draw maze content
  for (int i = 0; i < N; i++)
  {
    mvprintw(i + 5, 0, "|"); // Left border
    for (int j = 0; j < N; j++)
    {
      if (maze[i][j] == 1)
      {
        attron(COLOR_PAIR(8));
        mvprintw(i + 5, j * 3 + 1, "###"); // Wall
        attroff(COLOR_PAIR(8));
      }
      else if (global_visited[i][j] != -1)
      {
        attron(COLOR_PAIR(global_visited[i][j] + 1) | A_BOLD);
        mvprintw(i + 5, j * 3 + 1, " O "); // Explored path
        attroff(COLOR_PAIR(global_visited[i][j] + 1) | A_BOLD);
      }
      else
      {
        mvprintw(i + 5, j * 3 + 1, "   "); // Open space
      }
    }
    mvprintw(i + 5, N * 3 + 1, "|"); // Right border
  }

  // Draw bottom border
  mvprintw(N + 5, 0, "+");
  for (int j = 0; j < N * 3; j++)
    mvprintw(N + 5, j + 1, "-");
  mvprintw(N + 5, N * 3 + 1, "+");
  attroff(COLOR_PAIR(7) | A_BOLD);

  // Draw exit marker
  attron(COLOR_PAIR(5) | A_BOLD);
  mvprintw(EXIT_X + 5, EXIT_Y * 3 + 1, " E ");
  attroff(COLOR_PAIR(5) | A_BOLD);

  // Draw legend
  mvprintw(N + 7, 0, "+--------------------- Legend ---------------------+");

  attron(COLOR_PAIR(8));
  mvprintw(N + 8, 2, "###");
  attroff(COLOR_PAIR(8));
  printw(" = Wall");

  attron(COLOR_PAIR(5) | A_BOLD);
  mvprintw(N + 8, 20, " E ");
  attroff(COLOR_PAIR(5) | A_BOLD);
  printw(" = Exit");

  // Draw status message if maze is solved
  if (maze_solved)
  {
    attron(COLOR_PAIR(2) | A_BOLD); // Green text
    mvprintw(N + 10, 0, "Maze solved! Press ENTER to show final summary...");
    attroff(COLOR_PAIR(2) | A_BOLD);
  }

  refresh();
  if (!maze_solved)
  {
    usleep(50000); // 50ms delay for animation only while solving
  }
}

// Modified to print to terminal instead of ncurses
void print_final_summary(int winner_rank, double total_time)
{
  printf("\n========================================\n");
  printf("           MAZE SOLVER SUMMARY          \n");
  printf("========================================\n\n");

  printf("🏆 Process %d found the exit!\n", winner_rank);
  printf("⏱  Time taken: %.2f seconds\n", total_time);
  printf("========================================\n");
}

void explore(int start_x, int start_y, int rank, int size)
{
  int visited[N][N] = {0};
  top = -1;
  push(start_x, start_y);

  while (top >= 0 && !found_exit)
  {
    int x, y;
    pop(&x, &y);
    if (visited[x][y])
      continue;
    visited[x][y] = 1;

    // Check if exit is found first
    if (x == EXIT_X && y == EXIT_Y)
    {
      found_exit = 1;
      if (rank == 0)
      {
        global_visited[x][y] = rank;
        draw_maze(rank, 1); // Show solved state
      }
      else
      {
        int coord[2] = {x, y};
        MPI_Send(coord, 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
        int exit_msg = 1;
        MPI_Send(&exit_msg, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
      }
      break;
    }

    // Update visualization or notify rank 0
    if (rank == 0)
    {
      global_visited[x][y] = rank;
      draw_maze(rank, 0); // Show solving state
    }
    else
    {
      int coord[2] = {x, y};
      MPI_Send(coord, 2, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    // Push unvisited neighbors
    for (int i = 3; i >= 0; i--)
    {
      int new_x = x + moves[i][0];
      int new_y = y + moves[i][1];
      if (new_x >= 0 && new_x < N && new_y >= 0 && new_y < N &&
          maze[new_x][new_y] == 0 && !visited[new_x][new_y])
      {
        push(new_x, new_y);
      }
    }

    // Rank 0 processes incoming messages
    if (rank == 0)
    {
      int flag;
      MPI_Status status;
      while (1)
      {
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
        if (!flag)
          break;

        if (status.MPI_TAG == 0)
        { // Cell visited
          int coord[2];
          MPI_Recv(coord, 2, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          global_visited[coord[0]][coord[1]] = status.MPI_SOURCE;
          draw_maze(rank, 0); // Show solving state
        }
        else if (status.MPI_TAG == 1)
        { // Exit found
          int exit_msg;
          MPI_Recv(&exit_msg, 1, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          found_exit = 1;
          draw_maze(rank, 1); // Show solved state
          break;
        }
      }
    }
    else
    { // Other ranks check for stop signal
      int flag;
      MPI_Status status;
      MPI_Iprobe(0, 2, MPI_COMM_WORLD, &flag, &status);
      if (flag)
      {
        int stop;
        MPI_Recv(&stop, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        found_exit = 1;
        break;
      }
    }
  }

  // Rank 0 notifies others to stop
  if (rank == 0 && found_exit)
  {
    for (int i = 1; i < size; i++)
    {
      int stop = 1;
      MPI_Send(&stop, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
    }
  }
}

int main(int argc, char **argv)
{
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // Get start time
  if (rank == 0)
  {
    start_time = MPI_Wtime();
  }
  MPI_Bcast(&start_time, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // Initialize ncurses and colors for rank 0
  if (rank == 0)
  {
    initscr();
    noecho();
    curs_set(0);
    start_color();

    // Process colors
    init_pair(1, COLOR_RED, COLOR_BLACK);     // Process 0
    init_pair(2, COLOR_GREEN, COLOR_BLACK);   // Process 1
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);  // Process 2
    init_pair(4, COLOR_BLUE, COLOR_BLACK);    // Process 3
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK); // Exit marker

    // UI element colors
    init_pair(6, COLOR_CYAN, COLOR_BLACK);  // UI elements
    init_pair(7, COLOR_WHITE, COLOR_BLACK); // Borders
    init_pair(8, COLOR_WHITE, COLOR_WHITE); // Walls

    memset(global_visited, -1, sizeof(global_visited));
  }

  // Adjusted starting positions (all open cells)
  int start_positions[4][2] = {{0, 0}, {0, 2}, {2, 0}, {3, 0}};

  if (rank < 4)
  {
    int start_x = start_positions[rank][0];
    int start_y = start_positions[rank][1];
    if (maze[start_x][start_y] == 0)
    {
      explore(start_x, start_y, rank, size);
    }
  }

  // Wait for user input in rank 0 before showing summary
  if (rank == 0 && found_exit)
  {
    int ch;
    nodelay(stdscr, FALSE); // Make getch blocking
    while ((ch = getch()))
    {
      if (ch == '\n') // Only ENTER to exit
        break;
    }
    endwin();
  }

  // Gather final statistics
  double end_time = MPI_Wtime();
  int winner_rank = -1;

  // Find which process found the exit
  if (rank == 0)
  {
    MPI_Status status;
    int flag;
    MPI_Iprobe(MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &flag, &status);
    if (flag)
    {
      winner_rank = status.MPI_SOURCE;
    }
    else if (found_exit)
    {
      winner_rank = 0;
    }
  }
  MPI_Bcast(&winner_rank, 1, MPI_INT, 0, MPI_COMM_WORLD);

  // Print final summary only from rank 0
  if (rank == 0)
  {
    print_final_summary(winner_rank, end_time - start_time);
  }

  MPI_Finalize();
  return 0;
}