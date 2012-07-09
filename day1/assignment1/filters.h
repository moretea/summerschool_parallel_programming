/*
 * Summerschool Getting Moore From Multicores
 *
 * Assignment 1 - Introduction to image filters.
 *
 * 30-06-2011 filters.h
 *
 * (c) 2011 Roy Bakker & Raphael 'kena' Poss - University of Amsterdam
 *     Based on a framework by Aram Visser and Roeland Douma
 */

#ifndef FILTERS_H
#define FILTERS_H

/*
 * For more comments and details see the C file.
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ppm.h"

/* Gives you the chance to modify the input. */
void handle_input(char *input, img_t **in, img_t **out, img_t **original);

/* Gives you the chance to modify the output. */
void handle_output(char *output, img_t *in, img_t *out, img_t *original);

/* Applies a convolution with (<kx> * <ky>) <kernel> on <in> to <out>. */
img_t *apply(size_t kx, size_t ky, int *kernel, img_t *in, img_t *out);

/* Rotates an image over 90 degrees. */
img_t *rotate90(img_t *in, img_t *out);

/* Flips an image horizontally. */
img_t *fliph(img_t *in, img_t *out);

/* Flips an image virtually. */
img_t *flipv(img_t *in, img_t *out);

/* invert the colors of an image. */
img_t *invert(img_t *in, img_t *out);

/* Blur a bit. */
img_t *blur9(img_t *in, img_t *out);

/* Blur a bit more. */
img_t *blur25(img_t *in, img_t *out);

/* Apply the sobel algorithm for edge detection. */
img_t *sobel(img_t *in, img_t *out);

/* 8Sharpen an image */
img_t *sharpen(img_t *in, img_t *out);

/* Identity, or copy, or whatever you would like to call it. */
img_t *identity(img_t *in, img_t *out);

/* Improve the contrast of the image */
img_t *contrast(img_t *in, img_t *out);

/* Some default RGB and greyscale filters. */
img_t *red(img_t *in, img_t *out);
img_t *green(img_t *in, img_t *out);
img_t *blue(img_t *in, img_t *out);
img_t *gray(img_t *in, img_t *out);

/* Do something funny with the image. */
img_t *yourfilter1(img_t *in, img_t *out);
img_t *yourfilter2(img_t *in, img_t *out);
img_t *yourfilter3(img_t *in, img_t *out);

#endif

