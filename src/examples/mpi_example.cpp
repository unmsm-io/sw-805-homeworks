#include <iostream>
#include <mpi.h>

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv); // Initialize MPI

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get process rank
  MPI_Comm_size(MPI_COMM_WORLD, &size); // Get total number of processes

  std::cout << "Hello from process " << rank << " out of " << size << std::endl;

  MPI_Finalize(); // Clean up MPI
  return 0;
}
