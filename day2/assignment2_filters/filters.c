/*
 * Summerschool Getting Moore From Multicores
 *
 * Assignment 1 - Introduction to image filters.
 *
 * 30-06-2011 filters.c
 *
 * (c) 2011 Roy Bakker & Raphael 'kena' Poss - University of Amsterdam
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
 *   for(size_t y = 0; i < in->h; y++){
 *     for(size_t x = 0; x < w; x++){
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

#define INIT_DECLS \
  const pixel_t * restrict in_data = in->data; \
  pixel_t * restrict out_data = out->data; \
  const size_t in_w = in->w, in_h = in->h;
  

/* The identity just copies the image. */
img_t *identity(img_t *in, img_t *out){
  INIT_DECLS;

  /* Loop over all pixels and copy the values. */
  for(size_t i = 0; i < in_w * in_h; i++){
    out_data[i] = in_data[i];
  }

  /* Return new image. */
  return(out);
}


/* Invert every pixel in the img_t */
img_t *invert(img_t *in, img_t *out){
  INIT_DECLS;

  /* Loop over all pixels and invert the rgb values. */
  for(size_t i = 0; i < in_w * in_h; i++){
    out_data[i].r = 255 - in_data[i].r;
    out_data[i].g = 255 - in_data[i].g;
    out_data[i].b = 255 - in_data[i].b;
  }

  /* Return new image. */
  return(out);
}

/* Make it green. */
img_t *green(img_t *in, img_t *out){
  INIT_DECLS;

  /* Loop over all pixels and copy the green value. */
  for(size_t j = 0; j < in_h; j++){
    for(size_t i = 0; i < in_w; i++){
      out_data[j*in_w + i].r = 0;
      out_data[j*in_w + i].g = in_data[j*in_w + i].g;
      out_data[j*in_w + i].b = 0;
    }
  }

  /* Return new image. */
  return(out);
}


/* Make it blue. */
img_t *blue(img_t *in, img_t *out){
  INIT_DECLS;

  /* Loop over all pixels and copy the blue value. */
  for(size_t i = 0; i < in_w; i++){
    for(size_t j = 0; j < in_h; j++){
      out_data[j*in_w + i].r = 0;
      out_data[j*in_w + i].g = 0;
      out_data[j*in_w + i].b = in_data[j*in_w + i].b;
    }
  }

  /* Return new image. */
  return(out);
}

/* Make it red, but do it in paralell, we're multimoore totday! */
img_t *red(img_t *in, img_t *out){
  INIT_DECLS;

  /* Loop over all pixels and copy the red value. */
  #pragma omp parallel for
  for(size_t i = 0; i < in_w * in_h; i++){
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
  for(size_t i = 0; i < in_w * in_h; i++){

    /* The intensity of the pixel is taken after the YPbPr color model. */
    /* This gives more weight to green because the human eye is more sensitive to this color. */
    /* See http://www.equasys.de/colorconversion.html for details. */
    uint8_t cy = (in_data[i].r * 0.3 + in_data[i].g * 0.59 + in_data[i].b * 0.11);
    out_data[i].r = cy;
    out_data[i].g = cy;
    out_data[i].b = cy;
  }

  /* Return new image. */
  return(out);
}

/* Flip an image along the horizontal axis */
img_t *fliph(img_t *in, img_t *out){
  INIT_DECLS;

  /* Return new image. */
  return(out);
}


/* Flip an image along the vertical axis */
img_t *flipv(img_t *in, img_t *out){
  INIT_DECLS;
 
  /* Return new image. */
  return(out);
}

/*
 * Apply
 *
 * This is a helper function for all stencil (convolution) operations
 * used by various filters.
 *
 * Implementing this one needs knowledge about filters and some real good
 * consideration of array programming.
 *
 * Apply a <kw> times <kh> stencil operation with kernel <kernel> to the 
 * image <in>, and put the result in image <out>.
 */
img_t *apply(size_t kw, size_t kh, int* kernel, img_t *in, img_t *out){
  INIT_DECLS;

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

  /* Fill in the actual body. */

  /* Clean up the mess we have created. */
  deletePPM(out2);

  /* Return new image. */
  return(out);
}

/* Rotate an image by 90 degrees (clockwise, or anti-clockwise) */
img_t *rotate90(img_t *in, img_t *out){
  INIT_DECLS;

  /* Return new image. */
  return(out);
}

/*
 * Apply a filter to the image that makes the contrast better.
 * 
 *  Find the minimum and maximum value for brightness (see conversion to gray).
 *  Let Range = Max - Min
 *  Let (float) Scalingfactor = 255.0 / Range
 *  For each pixel P:
 *     compute old brightness like in the conversion to gray.
 *     compute new brightness = (old brightness - Min) * Scalingfactor.
 *     compute new R,G,B values from the new brighness.
 *     (see http://www.equasys.de/colorconversion.html for the formula)
 *   
 *  This should be done on the intensity (brightness) of each pixel,
 *  ie not the individual R,G,B component. For greyscale images,
 *  Both R,G and B are equal to the brightness so the computation
 *  is simplified. Otherwise, a different color space must be used,
 *  such as HSV, HSL, or YCbCr.
 */
img_t *contrast(img_t *in, img_t *out){
  INIT_DECLS;

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

  /* Return new image. */
  return(out);
}


/* Think of something funny, or usefull 1. */
img_t *yourfilter1(img_t *in, img_t *out){
  INIT_DECLS;

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



