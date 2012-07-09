/*
 * Summerschool Getting Moore From Multicores
 *
 * Assignment 1 - Introduction to image filters.
 *
 * 30-06-2011 timer.c
 *
 * (c) 2011 Roy Bakker & Raphael 'kena' Poss - University of Amsterdam
 */

#include <stdlib.h>
#include <stdio.h>
#include "timer.h"

/* Allocate memory for a timer, and return a pointer. */
timer *createRTimer() {

  /* Allocate and check meory. */
  timer *t = (timer*)malloc(sizeof(timer));

  if(!t) {
      perror("malloc");
      exit(1);
  }

  /* Return pointer. */
  return(t);
}

/* Start timer. */
void startRTimer(timer *t) {

  /* Put the start time in the appropriate struct. */
  gettimeofday(&t->start, NULL);
}

/* Stop timer. */
void stopRTimer(timer *t) {

  /* Put the endingtime in the appropriate struct. */
  gettimeofday(&t->stop, NULL);
}

/* Get the timer value as a double */
double getRTimer(timer *t) {

  /* Calculate seconds. */
  double sec = t->stop.tv_sec - t->start.tv_sec;

  /* Calculate microseconds. */
  double usec = t->stop.tv_usec - t->start.tv_usec;
  usec /= 1e6;

  /* Return time. */
  return(sec + usec);
}

/* Function to cleanup properly. */
void destroyRTimer(timer *t) {

  /* Release the memory. */
  free(t);
}
