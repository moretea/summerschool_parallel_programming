/*
 * Summerschool Getting Moore From Multicores
 *
 * Assignment 1 - Introduction to image filters.
 * 
 * 30-06-2011 ppm.c
 *
 * (c) 2011 Roy Bakker & Raphael 'kena' Poss - University of Amsterdam
 */

/* 
 * PPM manipulation functions
 * Base is copied from http://stackoverflow.com/questions/2693631/
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "ppm.h"

/* Reads in a PPm file and stores as img_t with pixel_ts as datablock. */
img_t *readPPM(const char *filename)
{
    /* Open file for input. */
    int fd = open(filename, O_RDONLY);
    if(fd == -1){
        perror(filename);
        exit(1);
    }   

    /* Figure out the size by seeking to the end, and back. */
    struct stat st;
    if (fstat(fd, &st) == -1){
        perror(filename);
        exit(1);
    }
    size_t sz = st.st_size;

    /* Allocate a buffer */
    char *buffer = (char*)malloc(sz + 1);
    if (!buffer){
        perror("malloc");
        exit(1);
    }

    /* Load the data from the file */
    size_t fs = 0;
    while (fs < sz)
    {
        /* read() might read less bytes than requested, check it. */
        ssize_t r = read(fd, buffer + fs, sz - fs);
        if (r < 0) {
            perror(filename);
            exit(1);
        }
        fs += r;
    }
    close(fd);

    /* Make sure that strings are nul-terminated if there is no data. */
    buffer[sz] = '\0';
    

    /* Check the image format. */
    size_t p = 0;
    if(p + 3 > sz || buffer[p] != 'P' || buffer[p + 1] != '6' || buffer[p + 2] != '\n'){
        fprintf(stderr, "Invalid image format (must be 'P6')\n");
        exit(1);
    }
    p += 3;

    /* Check for comments. */
    while(p < sz && buffer[p] && buffer[p] == '#'){
        while(p < sz && buffer[p] && buffer[p] != '\n')
            ++p;
        ++p;
    }

    /* Read the size. */
    char *endp;

    size_t w, h;
    
    if ((w = strtoul(buffer + p, &endp, 10)) == 0 ||
        buffer[p = endp - buffer] != ' ' ||
        (h = strtoul(buffer + p, &endp, 10)) == 0 ||
        buffer[p = endp - buffer] != '\n' ||
        strtoul(buffer + p, &endp, 10) != 255 ||
        buffer[p = endp - buffer] != '\n' ||
        sz - (p + 1) < w * h * 3){
        fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
        exit(1);
    }
    ++p;

    /* Memory allocation for the image. */
    img_t* img = (img_t*)malloc(sizeof(img_t));
    if (!img){
        perror("malloc");
        exit(1);
    }

    /* Memory allocation for pixel data. */
    pixel_t *img_data = (pixel_t*)malloc(w * h * sizeof(pixel_t));
    if(!img_data){
        perror("malloc");
        exit(1);
    }

    /* Read pixel data. */
    for (size_t i = 0; i < w * h; ++i){
        img_data[i].r = buffer[p + i * 3 + 0];
        img_data[i].g = buffer[p + i * 3 + 1];
        img_data[i].b = buffer[p + i * 3 + 2];
    }

    /* Clean up. */
    free(buffer);

    /* Return the image. */
    img->data = img_data;
    img->w = w;
    img->h = h;
    return img;
}

/* Create an image of size w * h */
img_t *createPPM(size_t w, size_t h){

    /* Allocate memory for image. */
    img_t *new = (img_t*)malloc(sizeof(img_t));
    if(!new){
        perror("malloc");
        exit(1);
    }

    /* Allocate memory for pixels. */
    new->data = (pixel_t*)calloc(w * h, sizeof(pixel_t));
    if(!new->data){
        perror("malloc");
        exit(1);
    }

    /* Set w and h */
    new->w = w;
    new->h = h;

    return new;
}

/* Copy an already read PPM image into a the same format. */
img_t *copyPPM(img_t *img){
    
    /* Allocate memory for image. */
    img_t *new = (img_t*)malloc(sizeof(img_t));
    if(!new){
        perror("malloc");
        exit(1);
    }

    /* Allocate memory for pixels. */
    new->data = (pixel_t*)malloc(img->w * img->h * sizeof(pixel_t));
    if(!new->data){
        perror("malloc");
        exit(1);
    }

    /* Set x and h */
    new->w = img->w;
    new->h = img->h;

    /* Copy pixel data. */
    memcpy((void*)new->data, (void*)img->data, img->w * img->h * sizeof(pixel_t));

    return new;
}

/* Write PPM data to a file. */
void writePPM(const char *filename, img_t *img)
{
    char *buffer;

    size_t w = img->w;
    size_t h = img->h;
    pixel_t *img_data = img->data;

    /* Open file for output. */
    int fd = open(filename, O_WRONLY|O_TRUNC|O_CREAT, 0666);
    if(fd == -1){
        perror(filename);
        exit(1);
    }   

    /* Prepare the output buffer. */
    buffer = (char*)malloc(32 + 3 * w * h);
    if (!buffer){
        perror("malloc");
        exit(1);
    }

    /* Generate the output image. */
    int offset = sprintf(buffer, "P6\n%zu %zu\n255\n", w, h);
    if (offset < 0){
        perror("sprintf");
        exit(1);
    }
    for (size_t i = 0; i < w * h; ++i){
        buffer[offset + i * 3 + 0] = img_data[i].r; 
        buffer[offset + i * 3 + 1] = img_data[i].g; 
        buffer[offset + i * 3 + 2] = img_data[i].b;
    } 

    /* Write the data to the file. */
    size_t sz = offset + 3 * w * h;
    size_t fs = 0;
    while (fs < sz)
    {
        /* write() might write less bytes than requested, check it */
        ssize_t r = write(fd, buffer + fs, sz - fs);
        if (r < 0) {
            perror(filename);
            exit(1);
        }
        fs += r;
    }

    /* Clean up. */
    close(fd);
    free(buffer);
}

/* Function added for proper cleanup. */
void deletePPM(img_t *img){
  /* Test for valid pointers and free data. */
  if(img){
      free(img->data);
  }
  free(img);
}

