#include <iostream>
#include <omp.h>

int main()
{
#pragma omp parallel
    {
        int id = omp_get_thread_num();

#pragma omp critical
        std::cout << "Hello from thread " << id << std::endl;
    }
    return 0;
}
