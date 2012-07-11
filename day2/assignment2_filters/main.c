/*
 * Summerschool Getting Moore From Multicores
 *
 * Assignment 1 - Introduction to image filters.
 * 
 * main.c 30-06-2011 
 *
 * (c) 2011 Roy Bakker & Raphael 'kena' Poss - University of Amsterdam
 *     Based on a framework by Aram Visser and Roeland Douma
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ppm.h"
#include "timer.h"
#include "filters.h"

static inline
size_t min(size_t a, size_t b)
{
    return a < b ? a : b;
}

/* The number of filters we have. */
#define NUM_FILTERS 17

/* The names of those filters. */
typedef img_t* (filter_func)(img_t* in, img_t* out);
struct filter_info
{
    const char *name;
    filter_func *func;
};

/* Information about all filters. */
                           
const struct filter_info FILTERS[NUM_FILTERS] = {
                    { "red", &red },
                    { "green", &green },
                    { "blue", &blue },
                    { "gray", &gray },
                    { "identity", &identity },
                    { "invert", &invert },
                    { "fliph", &fliph },
                    { "flipv", &flipv },
                    { "rotate90", &rotate90 },
                    { "blur9", &blur9 },
                    { "blur25", &blur25 },
                    { "sobel", &sobel },
                    { "contrast", &contrast },
                    { "sharpen", &sharpen },
                    { "yourfilter1", &yourfilter1 },
                    { "yourfilter2", &yourfilter2 },
                    { "yourfilter3", &yourfilter3 }
};

/*
 * The main function that parses the reads the imag
 */
int main(int argc, char **argv) {

  /* Timer and timing result. */
  timer *timer;
  double ttime, ltime, rtime, wtime;

  /* Number of times to apply the filter. */
  int iterations = 1;

  /* Input filename. */
  char *input = NULL;

  /* Output filename. */
  char *output = NULL;

  /* Name of the filter. */
  char *filter = "invert";

  /* Print help? */
  int help = 0;

  /* Pointers to the in and output images. */
  img_t *in, *out;

  /* Function pointer for the image filter. */
  filter_func *f = NULL;

  /* Parse command line arguments in a quick and easy way. */
  for(int i = 1; i < argc; i++){
    if(argv[i][0] == '-' && argv[i][1] != '\0' && argv[i][2] == '\0' && i < argc - 1){
      switch(argv[i][1]){
      case 'c': iterations = atoi(argv[i + 1]); break;
      case 'i': input  = argv[i + 1]; break;
      case 'o': output = argv[i + 1]; break;
      case 'f': filter = argv[i + 1]; break;
      }
      i++;
    }
    else{
      /* Unknown argument, or -h. */
      help = 1;
    }
  }

  /* Print help message. */
  if(help || !input || !output){
    printf(
      "Usage: %s -i INPUT -o OUTPUT [OPTIONS]...\n"
      "  -i FILE      Read the input image from FILE.\n"
      "  -o FILE      Write the output image to FILE.\n"
      "  -f NAME      Apply the filter with name NAME (default 'invert').\n"
      "  -c NUM       Apply the filter NUM times in sequence (default 1).\n"
      "  -h           Print this help.\n"
      "\n"
      "Valid filter names: "
      , argv[0]);

    for(int i = 0; i < NUM_FILTERS; i++){
      if(i % 3 == 2){ 
        printf("\n");
      }
      printf("%s ", FILTERS[i].name);
    } 

    printf("\n");

    return(0);
  }

  /* Set function pointer to correct filter. */
  for(int i = 0; i < NUM_FILTERS; i++){
    if(0 == strcmp(filter, FILTERS[i].name)){
      f = FILTERS[i].func;
    }
  }
  if (f == NULL){
      fprintf(stderr, "Unknown filter name: %s\n", filter);
      exit(1);
  }

  /* Create and benchmark timer. */
  timer = createRTimer();

  startRTimer(timer);
  stopRTimer(timer);
  ttime = getRTimer(timer);

  /* Print some information. */
  fprintf(stderr,
          "Parameters: -c %d -i %s -o %s -f %s\n"
          "Loading input... ", 
          iterations, input, output, filter);

  /* Read image and initialize an empty output image. */

  startRTimer(timer);
  in = readPPM(input);
  out = createPPM(in->w, in->h);
  stopRTimer(timer);
  ltime = getRTimer(timer);


  fprintf(stderr, "done.\nComputing... ");

  /* Apply the image filter iteration times. */
  startRTimer(timer);
  for(int i = 0; i < iterations; i++) {

    /* For multiple iterations, we need to swap input and output. */
    if(i != 0){
      img_t *tmp = in;
      in = out;
      out = tmp;
    }

    /* Call the actual filter function */
    f(in, out);
  }
  stopRTimer(timer);
  rtime = getRTimer(timer);

  fprintf(stderr, "done.\nWriting output... ");

  /* Write image to file. */
  startRTimer(timer);
  writePPM(output, out);
  stopRTimer(timer);
  wtime = getRTimer(timer);  

  fprintf(stderr, "done.\n");

  /* Print statistics. */
  printf("\"%s\" \"%s\" \"%s\" %d %f %f %f\n",
         input, output, filter, iterations, 
         ltime - ttime, 
         rtime - ttime, 
         wtime - ttime);
 
  /* Clean up. */
  destroyRTimer(timer);
  deletePPM(in);
  deletePPM(out);

  return(0);
}

