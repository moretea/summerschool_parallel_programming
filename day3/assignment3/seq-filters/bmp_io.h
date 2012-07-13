#ifndef __BMP_IO_H
#define __BMP_IO_H

typedef struct __color {
	float R;
	float G;
	float B;
} color_t;

typedef struct {
  float *R;
  float *G;
  float *B;
} fast_color_t;


typedef color_t *color_image_t;
typedef float* gray_image_t;
 
color_image_t read_BMP (const char* filename, int *imgW, int *imgH);
int write_GrayBMP(const char* filename, gray_image_t gray, int imgW, int imgH);
int write_BMP(const char* filename, color_image_t RGB, int imgW, int imgH);

#endif // __BMP_IO_H
