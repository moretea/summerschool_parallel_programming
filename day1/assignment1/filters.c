/*
 * Summerschool Getting Moore From Multicores
 *
 * Assignment 1 - Introduction to image filters.
 *
 * 30-06-2011 filters.c
 * 06-07-2012 Modified for MPI support.
 *
 * (c) 2011-2012 Roy Bakker & Raphael 'kena' Poss - University of Amsterdam
 *     Based on a framework by Aram Visser and Roeland Douma
 */

/*
 * Image filter skeletons file.
 *
 * Below are empty filter functions which you have to fill in.
 * Currently they do not modify the image. All filters assume that
 * image <in> and image <out> have the same (x, y), and pixel data has
 * already been allocated.
 * 
 *   img_t* in     a pointer to structure with the following members:
 *     in->w     The number of pixels in a row (width)
 *     in->h     The number of rows (height).
 *     in->data  A pointer to an array of (in->w * in->h) pixel structs.
 *
 *   A pixel struct consists of the members
 *     r        The red   value [0-255]
 *     g        The green value [0-255]
 *     b        The blue  value [0-255]
 *
 *   To access a pixel at location (x, y)'s red value we use the c code:
 *     in_data[y*in->w + x].r
 *
 *   To copy all pixel values in a naive way:
 *   for(size_t y = 0; i < in->h; y++) {
 *     for(size_t x = 0; x < w; x++) {
 *       out_data[y*w + x].r = in_data[y*w + x].r;
 *       out_data[y*w + x].g = in_data[y*w + x].g;
 *       out_data[y*w + x].b = in_data[y*w + x].b;
 *     }
 *   }
 *
 *   To copy all pixel values in a single loop:
 *   for(size_t i = 0; i < in->w * in->h; i++){
 *     out_data[i].r = in_data[i].r;
 *     out_data[i].g = in_data[i].g;
 *     out_data[i].b = in_data[i].b;
 *   }
 *
 */

#include "filters.h"
#include <string.h>

/* This makes it more easy and faster to access the image data. */
#define INIT_DECLS \
  const pixel_t * restrict in_data = in->data; \
  pixel_t * restrict out_data = out->data; \
  const size_t in_w = in->w, in_h = in->h;

/*
 * The actual image filters:
 */

/* The identity just copies the image. */
img_t *identity(img_t *in, img_t *out) {
  INIT_DECLS;

  /* Loop over all pixels and copy the values. */
  for (size_t i = 0; i < in_w * in_h; i++) {
    out_data[i] = in_data[i];
  }

  /* Return new image. */
  return(out);
}


/* Invert every pixel in the img_t */
img_t *invert(img_t *in, img_t *out) {
  INIT_DECLS;

  /* Loop over all pixels and invert the rgb values. */
  for (size_t i = 0; i < in_w * in_h; i++) {
    out_data[i].r = 255 - in_data[i].r;
    out_data[i].g = 255 - in_data[i].g;
    out_data[i].b = 255 - in_data[i].b;
  }

  /* Return new image. */
  return(out);
}

/*
 * Exercise 1.4 - 1: Compare the performance of the "green" and the "blue"
 * filter. Explain the difference.
 */

/* Make it green. */
img_t *green(img_t *in, img_t *out) {
  INIT_DECLS;

  /* Loop over all pixels and copy the green value. */
  for (size_t j = 0; j < in_h; j++) {
    for (size_t i = 0; i < in_w; i++) {
      out_data[j*in_w + i].r = 0;
      out_data[j*in_w + i].g = in_data[j*in_w + i].g;
      out_data[j*in_w + i].b = 0;
    }
  }

  /* Return new image. */
  return(out);
}

/* Make it blue. */
img_t *blue(img_t *in, img_t *out) {
  INIT_DECLS;

  /* Loop over all pixels and copy the blue value. */
  for (size_t j = 0; j < in_h; j++) {
    for (size_t i = 0; i < in_w; i++) {
      out_data[j*in_w + i].r = 0;
      out_data[j*in_w + i].g = 0;
      out_data[j*in_w + i].b = in_data[j*in_w + i].b;
    }
  }

  /* Return new image. */
  return(out);
}

/* 
 * Make it red. 
 *
 * Exercise 2.3 - 1: Do we need to modify this for use with MPI?
 *
 */
img_t *red(img_t *in, img_t *out) {
  INIT_DECLS;

  /* Loop over all pixels and copy the red value. */
  for (size_t i = 0; i < in_w * in_h; i++) {
    out_data[i].r = in_data[i].r;
    out_data[i].g = 0;
    out_data[i].b = 0;
  }

  /* Return new image. */
  return(out);
}


/* Make the image grayscale. */
img_t *gray(img_t *in, img_t *out){
  INIT_DECLS;

  /* Loop over all pixels and calculate mean value, set it */
  for (size_t i = 0; i < in_w * in_h; i++) {

    /* 
     * The intensity of the pixel is taken after the YPbPr color model.
     * This gives more weight to green because the human eye is more sensitive 
     * to this color.
     * See http://www.equasys.de/colorconversion.html for details. 
     */
    uint8_t cy = (in_data[i].r * 0.30 + 
                  in_data[i].g * 0.59 + 
                  in_data[i].b * 0.11 );

    out_data[i].r = cy;
    out_data[i].g = cy;
    out_data[i].b = cy;
  }

  /* Return new image. */
  return(out);
}

/* Flip an image along the horizontal axis */
img_t *fliph(img_t *in, img_t *out) {
  INIT_DECLS;

  for (size_t h = 0; h < in_h; h++) {
    for (size_t w = 0; w < in_w; w++) {

      size_t in_idx  = h * in_w + w;
      size_t out_idx = h * in_w + (in_w - w);

      out->data[out_idx] = in->data[in_idx];
    }
  }

  /* Return new image. */
  return(out);
}

/* Flip an image along the vertical axis */
img_t *flipv(img_t *in, img_t *out) {
  INIT_DECLS;

  for (size_t h = 0; h < in_h; h++) {
    for (size_t w = 0; w < in_w; w++) {

      size_t in_idx  = h              * in_w + w;
      size_t out_idx = (in_h - h - 1) * in_w + w;

      out->data[out_idx] = in->data[in_idx];
    }
  }
 

  /* Return new image. */
  return(out);
}

/*
 * Apply
 *
 * This is a helper function for all stencil (convolution) operations
 * used by various filters.
 *
 * Apply a <kw> times <kh> stencil operation with kernel <kernel> to the 
 * image <in>, and put the result in image <out>.
 *
 *
 * Exercise 2.4 - 2: Modify this function in such a way that it can handle
 *                   MPI execution. Hint: Where do we store the data for the 
 *                   boundaries that we need to exchange with our neighbours?
 *
 */
img_t *apply(size_t kw, size_t kh, int* kernel, img_t *in, img_t *out) {
  INIT_DECLS;

  /* Sanity check. */
  if ((kw % 2 == 0) || (kh % 2 == 0)) {
    printf(" Cannot apply kernel with even size!\n");
    return(out);
  }

  /* calculate the sum of the kernel, and do a zero check. */
  int sum = 0;  
  for (size_t i = 0; i < kw * kh; i++) {
    sum += kernel[i];
  }

  if (sum == 0) {
    sum = 1;
  }

  /* 
   * MPI: This might be a good point to get some data from the neighbours,
   *      And they might want to receive something from us as well...
   *
   *      How do we deal with the boundaries for the first and the last
   *      MPI process (rank 0 and rank n-1)?
   */

  /* Loop over all pixel rows. */
  for (size_t y = 0; y < in_h; y++) {

    /* Loop over all pixels in a row. */
    for (size_t x = 0; x < in_w; x++) {

      /* For readability calculate the current position in the image. */
      size_t imageindex = y * in_w + x;

      /* 
       * We need to check if we can apply the filter. If not (part of the 
       * kernel gets out of the image borders), just assign the input value.
       *
       * For MPI, we do need something special here. What?
       */
      if ((y <= (kh / 2)) || (y >= in_h - (kh / 2)) ||
         (x <= (kw / 2)) || (x >= in_w - (kw / 2)) ) {

        /* Copy input value. */
        out_data[imageindex] = in_data[imageindex];
      } else {
        /* Variables to sum up all colors during the loops over the kernel. */
        int r = 0;
        int g = 0;
        int b = 0;

        /* Loop over the kernel rows. */
        for (size_t suby = 0; suby < kh; suby++) {

          /* We need the offset from the center of the kernel. */
          size_t suby_centered = suby - (kh / 2);

          /* Loop over the kernel values in a row. */
          for(size_t subx = 0; subx < kw; subx++){

            /* We need the offset from the center of the kernel. */
            size_t subx_centered = subx - (kw / 2);

            /* Now we can calculate the index in the kernel array. */
            size_t kernelindex = suby * kw + subx;

            /* And the index in the image array. */
            size_t index = imageindex + (suby_centered * in_w) + subx_centered;

            /* Add the pixel values to our sums. */
            r += kernel[kernelindex] * in_data[index].r;
            g += kernel[kernelindex] * in_data[index].g;
            b += kernel[kernelindex] * in_data[index].b;
          }
        }

        if(r < 0) r = 0;
        if(g < 0) g = 0;
        if(b < 0) b = 0;

        /* Normalize and assign our new pixel. */
        out_data[imageindex].r = r / sum;
        out_data[imageindex].g = g / sum;
        out_data[imageindex].b = b / sum;
      }
    }
  }

  /* Return new image. */
  return(out);
}


/*
 * Blur the image by applying the following stencil to the image:
 *
 * 1 2 1
 * 2 3 2
 * 1 2 1
 */
img_t *blur9(img_t *in, img_t *out){

  /* Set the kernel. */
  int kernel[9] = {1, 2, 1,
                   2, 3, 2,
                   1, 2, 1
                  };

  /* Apply Kernel. */
  apply(3, 3, kernel, in, out);

  /* Return new image. */
  return(out);
}


/*
 * Blur the image by applying the following stencil to the image:
 *
 * 1  4  7  4 1
 * 4 20 33 20 4
 * 7 33 55 33 7
 * 4 20 33 20 4
 * 1  4  7  4 1
 */
img_t *blur25(img_t *in, img_t *out){

  /* Set the kernel. */
  int kernel[25] = {1,  4,  7,  4, 1,
                    4, 20, 33, 20, 4,
                    7, 33, 55, 33, 7,
                    4, 20, 33, 20, 4,
                    1,  4,  7,  4, 1
                   };

  /* Apply Kernel. */
  apply(5, 5, kernel, in, out);

  /* Return new image. */
  return(out);
}

/*
 * Do edge detection by applying the following stencils to the image and add
 * them:
 *
 *        1  2  1
 * Gx =   0  0  0
 *       -1 -2 -1   
 *
 *        1  0 -1
 * Gy =   2  0 -2
 *        1  0 -1
 *
 *  G =   sqrt(Gx^2 + Gy^2)
 *
 */
img_t *sobel(img_t *in, img_t *out){
  INIT_DECLS;

  /* 
   * We have to apply two filters from the same input,
   * so we have to create space for an other image.
   */
  img_t *out2 = copyPPM(in);

  /* Set the kernels. */
  int kernelX[9] = {-1, -2, -1,
                     0,  0,  0,
                     1,  2,  1
                   };

  int kernelY[9] = {-1,  0,  1,
                    -2,  0,  2,
                    -1,  0,  1
                   };

  /* Apply kernels. */
  apply(3, 3, kernelX, in, out);
  apply(3, 3, kernelY, in, out2);

  pixel_t * restrict out2_data = out2->data;

  /* Add results */
  for(size_t i = 0; i < in_w * in_h; i++){
    out2_data[i].r = sqrt((out_data[i].r ^ 2) + (out2_data[i].r ^ 2));
    out2_data[i].g = sqrt((out_data[i].g ^ 2) + (out2_data[i].g ^ 2));
    out2_data[i].b = sqrt((out_data[i].b ^ 2) + (out2_data[i].b ^ 2));
  }

  /* Make the contrast as hight as possible. */
  contrast(out2, out);

  /* Clean up the mess we have created. */
  deletePPM(out2);

  /* Return new image. */
  return(out);
}

/* Rotate an image by 90 degrees (clockwise, or anti-clockwise) */
img_t *rotate90(img_t *in, img_t *out) {
  INIT_DECLS;

  for (size_t y = 0; y < in_h; y++) {
    for (size_t x = 0; x < in_w; x++) {
      out_data[x * in_h + y] = in_data[y * in_w - x - 1];
    }
  }

  /* Need to swap the image width and height. */
  out->w = in_h;
  out->h = in_w;

  /* Return new image. */
  return(out);
}

/*
 * Apply a filter to the image that makes the contrast better.
 * 
 *  Find the minimum and maximum value (for brightness, see conversion to gray)
 *  Let Range = Max - Min
 *  Let (float) Scalingfactor_r = 255.0 / Range
 *  For each pixel P:
 *    P.r = (P.r - Min_r) * Scalingfactor_r
 *   
 *  This should be done on the intensity (brightness) of each pixel,
 *  ie not the individual R,G,B component. For greyscale images,
 *  Both R,G and B are equal to the brightness so the computation
 *  is simplified. Otherwise, a different color space must be used,
 *  such as HSV, HSL, or YCbCr.
 */
img_t *contrast(img_t *in, img_t *out){
  INIT_DECLS;

  uint8_t cymax = 0, cymin = 255;

  /* Loop over the rows of the image. */
  for(size_t y = 0; y < in_h; y++){

      /* Loop over the pixels in a row. */
      for(size_t x = 0; x < in_w; x++){

          /* Caluculate the index in the image array. */
          size_t i = y * in_w + x;
          
          /* The following formula provides the luma (intensity) of a color. */
          uint8_t cy = (in_data[i].r * 0.299 + 
                        in_data[i].g * 0.587 + 
                        in_data[i].b * 0.114 );

          /* Get minimum and maximum value. */
          if (cy > cymax) cymax = cy;
          if (cy < cymin) cymin = cy;
      }
  }

  /* Compute the dynamic range and scale factor for the luma. */
  uint8_t cyrange = cymax - cymin;
  float cy_scalefactor = 255.0 / cyrange;

  for (size_t i = 0; i < in_w * in_h; i++) {
      uint8_t r = in_data[i].r, g = in_data[i].g, b = in_data[i].b;

      /* Convert RGB to YCbCr (http://www.equasys.de/colorconversion.html). */
      float cy = (r *  0.299 + g *  0.587 + b *  0.114);
      float cb = (r * -0.169 + g * -0.331 + b *  0.500);
      float cr = (r *  0.500 + g * -0.419 + b * -0.081);

      /* Normalize luma according to dynamic range. */
      cy = (cy - cymin) * cy_scalefactor;

      /* Convert back to RGB. */
      out_data[i].r = cy               + cr *  1.400;
      out_data[i].g = cy + cb * -0.343 + cr * -0.711;
      out_data[i].b = cy + cb *  1.765;
  }

  /* Return new image. */
  return(out);
}


/*
 * Sharpen an image. 
 * This uses a convolution kernel like the following:
 *    -1 -1 -1
 *    -1 12 -1
 *    -1 -1 -1
 */
img_t *sharpen(img_t *in, img_t *out){
  INIT_DECLS;

  /* Set the kernel. */
  int kernel[9] = { 0, -1,  0,
                   -1,  12, -1,
                    0, -1,  0
                  };

  /* Apply Kernel. */
  apply(3, 3, kernel, in, out);

  /* Return new image. */
  return(out);
}

/* Think of something funny, or usefull 1. */
img_t *yourfilter1(img_t *in, img_t *out){
  INIT_DECLS;

  /* Exercise 2.4 - 3: Implement your filter here. */

  /* Return new image. */
  return(out);
}


/* Think of something funny, or usefull 2. */
img_t *yourfilter2(img_t *in, img_t *out){
  INIT_DECLS;

  /* Return new image. */
  return(out);
}


/* Think of something funny, or usefull 3. */
img_t *yourfilter3(img_t *in, img_t *out){
  INIT_DECLS;

  /* Return new image. */
  return(out);
}
