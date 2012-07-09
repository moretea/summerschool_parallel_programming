/*
 * Summerschool Getting Moore From Multicores
 *
 * Assignment 1 - Introduction to image filters.
 *
 * 30-06-2011 timer.h
 *
 * (c) 2011 Roy Bakker & Raphael 'kena' Poss - University of Amsterdam
 */

#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>

/*
 * A Very - very simple timer module.
 */

/* A timer only needs a start and stop value. */
typedef struct {
  struct timeval start;
  struct timeval stop;
} timer;

/* Create timer. */
timer *createRTimer(void);

/* Start timer. */
void startRTimer(timer *t);

/* Stop timer. */
void stopRTimer(timer *t);

/* Get value as double. */
double getRTimer(timer *t);

/* Clean up properly. */
void destroyRTimer(timer *t);
#endif
