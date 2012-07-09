#include "filters.h"
#include <string.h>

/* We can compile with and without MPI usage. */
#ifdef MPI
#  include <mpi.h>
#endif

void handle_input(char *input, img_t **in, img_t **out, img_t **original) {
#ifndef MPI
  /* Read input image, memory is automaticaly allocated. */
  img_t *read = readPPM(input);

  /* Allocate memory for output image. */
  *out = createPPM(read->w, read->h);

  /* For sequential code, the input is just what we read from file. */
  *in = read;
  *original = read;
#else
  /* Size and rank for MPI. */
  int size, rank;

  /* Array to broadcast image width and height. */
  int dim[2];

  /* Local variables for image size and number of lines to compute. */
  int w, h, extrarows, index;

  /* Declare pointer to image to read. */
  img_t *read = NULL;

  /* Need to ask MPI who we are, and how many processes there are. */
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  /* Only one (the first) process reads the input. */
  if (rank == 0) {

    /* Read input */
    read = readPPM(input);

    /* Get size for broadcasting to others */
    dim[0] = read->w;
    dim[1] = read->h;
  }

  /* Let others know the size of the image. */
  MPI_Bcast(dim, 2, MPI_INT, 0, MPI_COMM_WORLD);

  /* Width is the same for each process. */
  w = dim[0];

  /* We divide the rows of pixels (height) over the processes. */
  h = dim[1] / size;

  /*
   *  There might be some rows left (remainder of division),
   *  so some processes must do an additional row.
   */
  extrarows = dim[1] % size;
  if (rank < extrarows) {
    h++;
  }

  /* As we know the size by now, we can create the local images */
  *in = createPPM(w, h);
  *out = createPPM(w, h);

  /* The first one sends all the processes their data. */
  if (rank == 0) {

    /* First copy data for myself. */
    memcpy((*in)->data, read->data, w * h * sizeof(pixel_t));

    /* The next send should start at this index. */
    index = w * h;

    /* Loop over all workers. */
    for (int i = 1; i < size; i++) {

      /* Calculate how many rows this worker gets */
      int height = dim[1] / size;
      if (i < extrarows) {
        height++;
      }

      /* Do the actual sending */
      MPI_Send(read->data + index, w * height * sizeof(pixel_t),
               MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);

      /* Update the index for next send. */
      index += w * height;
    }

    /* Store a pointer to the original image. */
    *original = read;

  } else {
    /* All others just receive */

    MPI_Recv((*in)->data, w * h * sizeof(pixel_t),
             MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD, NULL);
  }
#endif
}

void handle_output(char *output, img_t *in, img_t *out, img_t *original) {
#ifndef MPI
  /* For sequential code, the data to write is in out. */
  img_t *write = out;

  /* Write data to disk. */
  writePPM(output, write);

  /* Free memory for both images. */
  deletePPM(in);
  deletePPM(out);
#else
  /* Size and rank for MPI. */
  int size, rank;

  /* Local variables for image size and number of lines to compute. */
  int w, h, extrarows, index;

  /* Declare pointer to image to write. */
  img_t *write = NULL;

  /* Need to ask MPI who we are, and how many processes there are. */
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);  

  /* Allocate memory for output image. */
  if (rank == 0) {
    write = createPPM(original->w, original->h);
  }

  /* Get the local width and height. */
  w = out->w;
  h = out->h;

  /* All slaves send their result to the master */
  if (rank > 0) {
    MPI_Send(out->data, w * h * sizeof(pixel_t), MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
  } else {
    int part_h     = original->h / size;
    int extra_rows = original->h % size;

    if (rank < extra_rows) {
      part_h ++;
    }

    int offset = original->w * part_h;
    memcpy(write->data, out->data, offset * sizeof(pixel_t));

    /* Receive each part from the slaves */
    for (int i = 1; i < size; i++) {
      int part_h     = original->h / size;
      int extra_rows = original->h % size;

      if (rank < extra_rows) {
        part_h ++;
      }

      int buffer_length = part_h * original->w * sizeof(pixel_t);
      pixel_t *tmp = write->data + offset;

      MPI_Recv(tmp, buffer_length,
          MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD, NULL);

      /* and copy the received bytes to the correct position */
      offset += buffer_length;
    }
  }

  /* Do the write-back once we have gathered all data. */
  if (rank == 0) {

    /* Write! */
    writePPM(output, write);

    /* ... and cleanup the memory */
    deletePPM(original);
    deletePPM(write);
  }

  /* Free memory for both images. */
  deletePPM(in);
  deletePPM(out);
#endif
}
