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

/* Utility function, use to do error checking.

   Use this function like this:

   checkCudaCall(cudaMalloc((void **) &deviceRGB, imgS * sizeof(color_t)));

   And to check the result of a kernel invocation:

   checkCudaCall(cudaGetLastError());
*/
static void checkCudaCall(cudaError_t result) {
    if (result != cudaSuccess) {
        cerr << "cuda error: " << cudaGetErrorString(result) << endl;
        exit(1);
    }
}


// Utility function, do not parallelize
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
    
    write_GrayBMP("../results/cuda/histogram.bmp", out, HISTOGRAM_SIZE, HISTOGRAM_SIZE);
    free(out);
}

static fast_color_t *convert_to_fast(color_image_t org, int size) {
  fast_color_t *fast = (fast_color_t*) malloc(sizeof(fast));

  fast->R = (float*) malloc(sizeof(float) * size);
  fast->G = (float*) malloc(sizeof(float) * size);
  fast->B = (float*) malloc(sizeof(float) * size);

  for (int i = 0; i < size; i++) {
    fast->R[i] = org[i].R;
    fast->G[i] = org[i].G;
    fast->B[i] = org[i].B;
  }

  return fast;
}

static fast_color_t *malloc_and_copy_device_img(fast_color_t *host_img, int size) {
  fast_color_t *device_img;

  device_img = (fast_color_t*) malloc(sizeof(fast_color_t));

  checkCudaCall(cudaMalloc( &(device_img->R), sizeof(float) * size));
  checkCudaCall(cudaMalloc( &(device_img->G), sizeof(float) * size));
  checkCudaCall(cudaMalloc( &(device_img->B), sizeof(float) * size));

  /* Copy */
  cudaMemcpy(device_img->R, host_img->R, sizeof(float) * size, cudaMemcpyHostToDevice);
  cudaMemcpy(device_img->G, host_img->G, sizeof(float) * size, cudaMemcpyHostToDevice);
  cudaMemcpy(device_img->B, host_img->B, sizeof(float) * size, cudaMemcpyHostToDevice);

  return device_img;
}

void free_device_img(fast_color_t *deviceImg) {
  cudaFree(deviceImg->R);
  cudaFree(deviceImg->G);
  cudaFree(deviceImg->B);
  free(deviceImg);
}

__global__ void gray_kernel(float *R, float *G, float *B, float *gray) {
  unsigned int i = (blockDim.x * blockIdx.x + threadIdx.x) * 4;

  /* Load R */
  float r1 = R[i];
  float r2 = R[i+1];
  float r3 = R[i+2];
  float r4 = R[i+3];

  /* Load G */
  float g1 = G[i];
  float g2 = G[i+1];
  float g3 = G[i+2];
  float g4 = G[i+3];

  /* Load B */
  float b1 = B[i];
  float b2 = B[i+1];
  float b3 = B[i+2];
  float b4 = B[i+3];

  /* Calc */
  gray[i]     = (30 * r1 + 59 * g1 + 11 * b1) / 100;
  gray[i + 1] = (30 * r2 + 59 * g2 + 11 * b2) / 100;
  gray[i + 2] = (30 * r3 + 59 * g3 + 11 * b3) / 100;
  gray[i + 3] = (30 * r4 + 59 * g4 + 11 * b4) / 100;
}


cudaEvent_t start, stop;
// Parallelize this function on the GPU
gray_image_t RGBtoGray(color_image_t RGB, int imgW, int imgH) {
    int imgS = imgW * imgH;
    gray_image_t gray;

    /* Convert image to a efficient float array*/
    fast_color_t *hostImg = convert_to_fast(RGB, imgS);

    /* Alloc & copy image to the GPU */
    fast_color_t *deviceImg = malloc_and_copy_device_img(hostImg, imgS);

    gray_image_t *device_out;
    checkCudaCall(cudaMalloc((void **) &device_out, sizeof(float) * imgS));
    cudaMemset(device_out,0, sizeof(float) * imgS);

    /* Compute gray */
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);
    gray_kernel<<< (imgS / 1024 / 4), 1024>>>(deviceImg->R, 
                                          deviceImg->G, 
                                          deviceImg->B, 
                                          (float*) device_out);
    cudaEventRecord(stop, 0);

    /* Copy back */
    gray = (gray_image_t) malloc(imgS * sizeof(float));
    cudaMemcpy(gray, device_out, imgS * sizeof(float), cudaMemcpyDeviceToHost);

    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime, start, stop);
    
    cout << "kernel invocation for rgbtogray took " << (elapsedTime) << " milliseconds" << endl;

    /* Free image */
    free_device_img(deviceImg);
    return gray;
}

// Parallelize this function on the GPU
void histogram_1D(gray_image_t image, int imgSize, unsigned int *histogram) {
    for (int i=0; i<HISTOGRAM_SIZE; i++) {
        histogram[i]=0;
    }

    for (int i=0; i<imgSize; i++) {
        histogram[(int)image[i]]++;
    }
}

// Parallelize this function on the GPU
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
                     

// Parallelize this function on the GPU
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


// Parallelize this function on the GPU
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
    cout << "MEH" << endl;
    rgbToGrayTimer.stop();

    free(image);
    write_GrayBMP("../results/cuda/gray.bmp", gray, imgW, imgH);

    // Compute Histogram
    histogramTimer.start();
    histogram_1D(gray, imgSize, histogram);
    histogramTimer.stop();

    save_histogram(histogram);
    

    // Contrast Enhancement
    contrastTimer.start();
    contrast_1D(gray, histogram, imgSize);
    contrastTimer.stop();

    write_GrayBMP("../results/cuda/contrast.bmp", gray, imgW, imgH);
    

    // Convolution
    convolutionTimer.start();
    gray_image_t outX = convolution2D(gray, imgW, imgH, SOBEL_FILTER_X);
    gray_image_t outY = convolution2D(gray, imgW, imgH, SOBEL_FILTER_Y);
    combineImages(outX, outY, imgSize);
    convolutionTimer.stop();

    write_GrayBMP("../results/cuda/convolution.bmp", outX, imgW, imgH);
    
    cout << rgbToGrayTimer;
    cout << histogramTimer;
    cout << contrastTimer;
    cout << convolutionTimer;

    free(gray);
    free(outX);
    free(outY);
    
    return 0;
}
