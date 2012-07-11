#include <math.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "bmp_io.h"

using namespace std;

#define THRESHOLD 16

void compareSizes(char* file1, char*file2) {
    FILE *img1, *img2;
    long size1, size2;

    img1=fopen(file1, "r"); 
    if (img1 == NULL) {
	cerr << "Cannot open " << file1 << ", bailing out" << endl;
	exit(1);
    }
    
    img2=fopen(file2, "r"); 
    if (img2 == NULL) {
	cerr << "Cannot open " << file2 << ", bailing out" << endl;
	fclose(img1); 
	exit(1);
    }
    fseek(img1, 0, SEEK_END); size1 = ftell(img1); 
    fseek(img2, 0, SEEK_END); size2 = ftell(img2); 
    
    if (size1>size2) {
	cerr << "File " << file1 << " is bigger than file " << file2 << endl;
	exit(1);
    } else if (size1<size2) {
	cerr << "File " << file2 << " is bigger than file " << file1 << endl;
	exit(1);
    }

    fclose(img1);
    fclose(img2);
}


int main(int argc, char* argv[]) {
    if (argc<3)   {
	cerr << "Not enough arguments, bailing out..." << endl;
	return -1;
    }

    compareSizes(argv[1], argv[2]);

    color_image_t image1;
    color_image_t image2;
    int imgW1, imgH1, imgW2, imgH2;

    if ((image1 = read_BMP(argv[1], &imgW1, &imgH1)) == NULL) {
	cerr << "Cannot read BMP1..." << endl;
	return 1;	
    }

    if ((image2 = read_BMP(argv[2], &imgW2, &imgH2)) == NULL) {
	cerr << "Cannot read BMP2..." << endl;
	return 1;	
    }

    if(imgW1 != imgW2) {
	cerr << "Width of the two images is not the same!" << endl;
    }

    if(imgH1 != imgH2) {
	cerr << "Height of the two images is not the same!" << endl;
    }

    int imgSize = imgW1 * imgH1;
    color_image_t diff = (color_image_t) malloc(imgSize * sizeof(color_t));
    if(diff == NULL) {
	cerr << "out of memory" << endl;
	return 1;
    }

    int pixelsAboveThreshold = 0;

    double total_fault = 0;

    for(int i=0; i<imgSize; i++) {
      diff[i].R = fabsf(image1[i].R - image2[i].R);
      diff[i].G = fabsf(image1[i].G - image2[i].G);
      diff[i].B = fabsf(image1[i].B - image2[i].B);

      total_fault = diff[i].R + diff[i].G + diff[i].B;

      if (diff[i].R > THRESHOLD || diff[i].G > THRESHOLD || diff[i].B > THRESHOLD) {
          pixelsAboveThreshold++;
      }
    }

    write_BMP("diff.bmp", diff, imgW1, imgH1);

    float percent = (100.0f * pixelsAboveThreshold) / imgSize;
    cout << "pixels above threshold: " << pixelsAboveThreshold << "(" << percent << " \%%)" << endl;
    cout << "Total diff:" << total_fault << endl;

    if (total_fault > 0) {
      return 1;
    } else {
      return 0;
    }
}
