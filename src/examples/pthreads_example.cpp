#include <iostream>
#include <pthread.h>

void *printMessage(void *id)
{
  long tid = (long)id;
  std::cout << "Hello from thread " << tid << std::endl;
  pthread_exit(nullptr);
}

int main()
{
  pthread_t threads[4];

  for (long i = 0; i < 4; i++)
  {
    pthread_create(&threads[i], nullptr, printMessage, (void *)i);
  }

  for (int i = 0; i < 4; i++)
  {
    pthread_join(threads[i], nullptr);
  }

  return 0;
}
