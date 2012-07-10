#include <omp.h>
#include <stdio.h>

int main() {
  omp_set_num_threads(8);

  int nr_threads = 0;

  #pragma omp parallel shared (nr_threads)
  {
    if (omp_get_thread_num() == 0) {
      #pragma omp critical (nr_threads)
      {
        nr_threads = omp_get_num_threads();
      }
    } else {
      int do_break = 0;
      while (!do_break) {
        #pragma omp critical (nr_threads)
        {
          do_break = nr_threads > 0;
        }
      }
    }

    printf("Hello, I'm thread %i of %i\n",  omp_get_thread_num(), nr_threads);
  }

  return 0;
}
