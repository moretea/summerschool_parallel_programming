/*
 * Summerschool Getting Moore From Multicores
 *
 * Assignment 1 - Introduction to image filters.
 *
 * 30-06-2011 ppm.h
 *
 * (c) 2011 Roy Bakker & Raphael 'kena' Poss - University of Amsterdam
 */

/* 
 * PPM manipulation functions
 * Base is copied from http://stackoverflow.com/questions/2693631/
 */

#ifndef PPM_H
#define PPM_H

#include <stdint.h>

/* 
 * Structure for a pixel.
 *
 * We might want to do this a bit different though...
 *
 */ 
typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} pixel_t;

/* Structure for an image. */
typedef struct {
  size_t w;
  size_t h;
  pixel_t *data;
} img_t;

/* Read the file <filename> and stores it;s PPM data into an array. */
img_t *readPPM(const char *filename);

/* Create an image of size w * h */
img_t *createPPM(size_t w, size_t h);

/* Makes an exact copy of an image. */
img_t *copyPPM(img_t *img);

/* Writes the data of <img> to the file <filename> */
void writePPM(const char *filename, img_t *img);

/* Cleans up the data of an image. */
void deletePPM(img_t *img);

#endif

