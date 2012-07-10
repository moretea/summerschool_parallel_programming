#include <omp.h>
#include <stdio.h>

int main() {
  omp_set_num_threads(8);

  #pragma omp parallel
  {
    if (omp_get_thread_num() == 0) {
      printf("There are %i threads\n", omp_get_num_threads());
    }

    printf("Hello, I'm thread %i\n",  omp_get_thread_num());
  }

  return 0;
}
