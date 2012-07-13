#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

static double timer_to_sec(struct timeval start, struct timeval stop) {

  /* Calculate seconds. */
  double sec = stop.tv_sec - start.tv_sec;

  /* Calculate microseconds. */
  double usec = stop.tv_usec - start.tv_usec;
  usec /= 1e6;

  /* Return time. */
  return(sec + usec);
}

int main() {
  const int N = 100;
  const int ITERATIONS = 10;
  unsigned int arr[N];
  int i, iter;
  unsigned int result;

  struct timeval start;
  struct timeval stop;

  /* Fill array with values */
  for (i = 0; i < N; i++) {
    arr[i] = i;
  }

  iter = 0;

  gettimeofday(&start, NULL);
  while (iter++ < ITERATIONS) {
    result = 0;

    /* Calculate result */
    #pragma omp parallel for
    for (i = 0; i < N; i++) {
      #pragma omp critical
      result += arr[i];
    }
  }
  gettimeofday(&stop, NULL);

  double total_time = timer_to_sec(start, stop);
  printf("Result: %i, which took %fs to compute all iterations.\n"
         "Per iteration: %f\n", result, total_time, total_time / ITERATIONS);
}
