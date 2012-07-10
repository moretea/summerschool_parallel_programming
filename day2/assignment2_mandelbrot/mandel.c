#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

static const double magnify = 1.5;    /* no magnification          */
static const int    itermax = 10000;  /* how many iterations to do */
static const int    hxres   = 500;   /* horizonal resolution      */
static const int    hyres   = 500;   /* vertical resolution       */

static const int ITERATIONS = 10;

void do_put_pixel(int red, int green, int blue) {
  fputc((char) red,   stdout);
  fputc((char) green, stdout);
  fputc((char) blue,  stdout);
}

void nop_put_pixel(int red, int green, int blue) {
  /* NOP */
}

void print_header(int height, int width) {
    /* header for PPM output */
    printf("P6\n");
    printf("%d %d\n255\n", height, width);
}

void calc_mandelbrot_seq(void (*put_pixel)(int, int, int)) {
  double x, xx, y, cx, cy;
  int iteration;

  for (int hy=1; hy<=hyres; hy++)  {
    for (int hx=1; hx<=hxres; hx++)  {
      cx = (( (float) hx) / ((float) hxres) -0.5) / magnify * 3.0 - 0.5;
      cy = (( (float) hy) / ((float) hyres) -0.5) / magnify * 3.0;

      x = 0.0;
      y = 0.0;
      
      for (iteration=1; iteration < itermax; iteration++) {
        xx = x*x-y*y+cx;
        y = 2.0*x*y+cy;
        x = xx;
        if (x*x+y*y>100.0) {
          iteration = 999999;
        }
      }

      if (iteration < 99999) {
        put_pixel(0, 128, 255);
      } else {
        put_pixel(0, 0, 0);
      }
    }
  }
}

void calc_mandelbrot_global(void (*put_pixel)(int, int, int)) {
  double x, xx, y, cx, cy;
  int iteration;

  int required_size = hyres * hxres * 3;
  char *buffer = malloc(required_size);

  if (buffer == NULL) {
    fprintf(stderr, "Couldn't allocate buffer of size %ib\n", required_size);
    exit(1);
  }

  #pragma omp parallel shared (buffer) private (x, y, cx, cy, xx, iteration)
  {
    #pragma omp for 
    for (int hy=1; hy<=hyres; hy++)  {
      for (int hx=1; hx<=hxres; hx++)  {
        cx = (( (float) hx) / ((float) hxres) -0.5) / magnify * 3.0 - 0.5;
        cy = (( (float) hy) / ((float) hyres) -0.5) / magnify * 3.0;

        x = 0.0;
        y = 0.0;
        
        for (iteration=1; iteration < itermax; iteration++) {
          xx = x*x-y*y+cx;
          y = 2.0*x*y+cy;
          x = xx;
          if (x*x+y*y>100.0) {
            iteration = 999999;
          }
        }

        unsigned char r, g, b;
        if (iteration < 99999) {
          r = 0;
          g = 128;
          b = 255;
        } else {
          r = 5;
          g = 8;
          b = 5;
        }

        /* Calculate offset */
        int offset = (hxres * (hy - 1) + (hx - 1)) * 3;
        buffer[offset]     = r;
        buffer[offset + 1] = g;
        buffer[offset + 2] = b;
      }
    }
  } /* End parallel */

  for (int offset = 0; offset < hxres*hyres*3; offset += 3) {
    put_pixel(buffer[offset], buffer[offset+1], buffer[offset+2]);
  }
}

void time(char *title, void (*func)(void (*put_pixel)(int, int, int))) {
  struct timeval start;
  struct timeval stop;

  fprintf(stderr, "Starting test for %s...\n", title);

  gettimeofday(&start, NULL);

  for(int i= 0; i < ITERATIONS; i++) {
    fprintf(stderr, "%s iteration %i\n", title, i);
    func(nop_put_pixel);
  }

  gettimeofday(&stop, NULL);

  double total_time = (stop.tv_sec - start.tv_sec) +  ((stop.tv_usec - start.tv_usec) / 1e6);
  fprintf(stderr, "%s took %fs to compute all iterations.\n"
         "Per iteration: %fs\n", title, total_time, total_time / ITERATIONS);
}

int main() {
//  time("Sequentionally",  calc_mandelbrot_seq);
  time("Optimize global",  calc_mandelbrot_global);

  if (0) {
    fprintf(stderr, "Calculating image\n");
    print_header(hxres, hyres);
    calc_mandelbrot_global(do_put_pixel);

    fprintf(stderr, "done!\n");
  }

  return 0;
}
