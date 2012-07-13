#include <math.h>
#include <iostream>
#include <stdlib.h>
#include "bmp_io.h"
#include "timer.h"

using namespace std;

#define HISTOGRAM_SIZE      256
#define CONTRAST_THRESHOLD   80
#define FILTER_WIDTH          3
#define FILTER_HEIGTH         3

float SOBEL_FILTER_X[FILTER_HEIGTH][FILTER_WIDTH] = { {-1,  0,  1}, {-2, 0, 2}, {-1, 0, 1} };
float SOBEL_FILTER_Y[FILTER_HEIGTH][FILTER_WIDTH] = { { 1,  2,  1}, { 0, 0, 0}, {-1,-2,-1} };


void save_histogram(unsigned int* histogram) {
    gray_image_t out = (gray_image_t)malloc(HISTOGRAM_SIZE*HISTOGRAM_SIZE*sizeof(float)); 

    unsigned max = 0;
    for (unsigned x=0; x<HISTOGRAM_SIZE; x++) {
	if(histogram[x] > max) max = histogram[x];
    }
	
    for (unsigned x=0; x<HISTOGRAM_SIZE; x++) {
	int val = HISTOGRAM_SIZE*histogram[x] / max;
	for (int y=0; y<val; y++) {
	    out[y*HISTOGRAM_SIZE + x] = 0xFF;
	}
	for (unsigned y=val; y<HISTOGRAM_SIZE; y++) {
	    out[y*HISTOGRAM_SIZE + x] = 0;
	}
    }
	
    write_GrayBMP("../results/seq/histogram.bmp", out, HISTOGRAM_SIZE, HISTOGRAM_SIZE);
    free(out);
}


gray_image_t RGBtoGray(color_image_t RGB, int imgW, int imgH) {
    int imgS = imgW * imgH;
    gray_image_t gray;

    // Y = 0.3*R + 0.59*G + 0.11*B
    gray = (gray_image_t) malloc(imgS * sizeof(float));
    if (gray == NULL) return NULL;

    for (int i = 0; i < imgS; i++) {
	gray[i] = (30*RGB[i].R + 59*RGB[i].G + 11*RGB[i].B) / 100;
    }

    return gray;
}


void histogram_1D(gray_image_t image, int imgSize, unsigned int *histogram) {
  
    for (int i=0; i<HISTOGRAM_SIZE; i++) {
	histogram[i]=0;
    }

    for (int i=0; i<imgSize; i++) {
	histogram[(int)image[i]]++;
    }
}


void contrast_1D(gray_image_t image, unsigned int *histogram, int imgSize) {	
    int i, min, max, diff;

    i=0;
    while (i<HISTOGRAM_SIZE && histogram[i] < CONTRAST_THRESHOLD) {
	i++;
    }
    min=i;

    i=HISTOGRAM_SIZE-1;
    while (i>min && histogram[i] < CONTRAST_THRESHOLD) { 
	i--;
    } 
    max=i;
    diff=max-min;
	
    for (i=0; i<imgSize; i++) { 
	if (image[i] < min) {
	    image[i]=0;
	} else if (image[i] > max) {
	    image[i]=255;
	} else {
	    image[i] = 255 * (image[i]-min) / (float)diff;
	}
    }
}
					 


gray_image_t convolution2D(gray_image_t in, int imgW, int imgH, float filter[FILTER_HEIGTH][FILTER_WIDTH]) {
    int imgS = imgW * imgH;
    gray_image_t out;

    out = (gray_image_t) calloc(imgS, sizeof(float)); // calloc initializes the memory to 0.
    if (out == NULL) return NULL;

    // find center position of kernel (half of kernel size)
    int kCenterX = FILTER_WIDTH / 2;
    int kCenterY = FILTER_HEIGTH / 2;

    for(int y=0; y < imgH; y++) {
	for(int x=0; x < imgW; x++) {
	    
	    for(int m=0; m < FILTER_HEIGTH; ++m) {
		for(int n=0; n < FILTER_WIDTH; ++n) {
		    
		    // index of input signal, used for checking boundary
		    int yy = y + (m - kCenterY);
		    int xx = x + (n - kCenterX);
		    
		    // ignore input samples which are out of bound
		    if( yy >= 0 && yy < imgH && xx >= 0 && xx < imgW ) {
			out[y*imgW+x] += in[yy*imgW+xx] * filter[m][n];
		    }
		}
	    }
	}
    }
    
    return out;
}


// Combines in1 and in2, stores result in image in1 (overwriting the original).
void combineImages(gray_image_t in1, gray_image_t in2, int imgSize) {
    for (int i=0; i<imgSize; i++) {
	in1[i] = sqrt(in1[i]*in1[i]+in2[i]*in2[i]);
	if(in1[i] > 255.0f) {
	    in1[i] = 255.0f;
	} 
    }
}


int main(int argc, char* argv[]) {
    int imgW, imgH;
    color_image_t image;
    unsigned int histogram[HISTOGRAM_SIZE];

    if (argc<2)   {
	cerr << "Not enough arguments! Bailing out..." << endl;
	return -1;
    }
	
    if ((image = read_BMP(argv[1], &imgW, &imgH)) == NULL) {
	cerr << "Cannot read BMP ... ?! " << endl;
	return 1;	
    }
    int imgSize = imgW * imgH;
    timer rgbToGrayTimer("rgb to gray");
    timer histogramTimer("histogram");
    timer contrastTimer("contrast");
    timer convolutionTimer("convolution");


    // Convert to grayscale image
    rgbToGrayTimer.start();
    gray_image_t gray = RGBtoGray(image, imgW, imgH);
    rgbToGrayTimer.stop();

    free(image);
    write_GrayBMP("../results/seq/gray.bmp", gray, imgW, imgH);
	

    // Compute Histogram
    histogramTimer.start();
    histogram_1D(gray, imgSize, histogram);
    histogramTimer.stop();

    save_histogram(histogram);
	

    // Contrast Enhancement
    contrastTimer.start();
    contrast_1D(gray, histogram, imgSize);
    contrastTimer.stop();

    write_GrayBMP("../results/seq/contrast.bmp", gray, imgW, imgH);
	

    // Convolution
    convolutionTimer.start();
    gray_image_t outX = convolution2D(gray, imgW, imgH, SOBEL_FILTER_X);
    gray_image_t outY = convolution2D(gray, imgW, imgH, SOBEL_FILTER_Y);
    combineImages(outX, outY, imgSize);
    convolutionTimer.stop();

    write_GrayBMP("../results/seq/convolution.bmp", outX, imgW, imgH);
	
    cout << rgbToGrayTimer;
    cout << histogramTimer;
    cout << contrastTimer;
    cout << convolutionTimer;

    free(gray);
    free(outX);
    free(outY);
	
    return 0;
}
