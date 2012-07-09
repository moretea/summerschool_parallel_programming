#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
  int rank, size;

  /* Initialize MPI (required) */

  MPI_Init(&argc, &argv);

  /* Get PID */
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  /* Get # process */

  MPI_Comm_size(MPI_COMM_WORLD, &size);

  /* Hello hello! */
  printf("Hello world from processe %d of %d\n", rank, size);

  /* Finalize MPI (required) */

  MPI_Finalize();

  return 0;
}
