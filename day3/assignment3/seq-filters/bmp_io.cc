#include "bmp_io.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

typedef struct { /**** BMP file info structure ****/
    unsigned int biSize; /* Size of info header */
    int biWidth; /* Width of image */
    int biHeight; /* Height of image */
    unsigned short biPlanes; /* Number of color planes */
    unsigned short biBitCount; /* Number of bits per pixel */
    unsigned int biCompression; /* Type of compression to use */
    unsigned int biSizeImage; /* Size of image data */
    int biXPelsPerMeter; /* X pixels per meter */
    int biYPelsPerMeter; /* Y pixels per meter */
    unsigned int biClrUsed; /* Number of colors used */
    unsigned int biClrImportant; /* Number of important colors */
} BITMAPINFOHEADER;

static const int BITMAPINFOHEADER_SIZE = 40;

typedef struct { /**** BMP file header structure ****/
    unsigned short bfType; /* Magic number for file */
    unsigned int bfSize; /* Size of file */
    unsigned short bfReserved1; /* Reserved */
    unsigned short bfReserved2; /* ... */
    unsigned int bfOffBits; /* Offset to bitmap data */
} BITMAPFILEHEADER;

static const int BITMAPFILEHEADER_SIZE = 14;

typedef struct { /**** Colormap entry structure ****/
    unsigned char rgbBlue; /* Blue value */
    unsigned char rgbGreen; /* Green value */
    unsigned char rgbRed; /* Red value */
    unsigned char rgbReserved; /* Reserved */
} RGBQUAD;

static BITMAPFILEHEADER header;
static BITMAPINFOHEADER info;


unsigned short /* O - 16-bit unsigned integer */
read_word(FILE *img1) { /* I - File to read from */
    unsigned char b0, b1; /* Bytes from file */

    b0 = getc(img1);
    b1 = getc(img1);
    return ((b1 << 8) | b0);
}


/*
 * 'read_dword()' - Read a 32-bit unsigned integer.
 */
unsigned int /* O - 32-bit unsigned integer */
read_dword(FILE *img1) { /* I - File to read from */
    unsigned char b0, b1, b2, b3; /* Bytes from file */

    b0 = getc(img1);
    b1 = getc(img1);
    b2 = getc(img1);
    b3 = getc(img1);
    return ((((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}


void safeRead(void* data, size_t size, size_t elementCount, FILE* file) {
    size_t res;
    size_t stillToRead = size * elementCount;

    do {
	res = fread(data, size, elementCount, file);
    
	if (res <= 0) {
	    cerr << "read error, res " << res << ", size = " << size << ", stillToRead = " << stillToRead << endl;
	    exit(0);
	}

	stillToRead -= res;

    } while(stillToRead != 0);
}


/*
 * 'read_long()' - Read a 32-bit signed integer.
 */
int /* O - 32-bit signed integer */
read_long(FILE *img1) { /* I - File to read from */
    unsigned char b0, b1, b2, b3; /* Bytes from file */

    b0 = getc(img1);
    b1 = getc(img1);
    b2 = getc(img1);
    b3 = getc(img1);

    return ((int) (((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}


/*
 * 'write_word()' - Write a 16-bit unsigned integer.
 */
int /* O - 0 on success, -1 on error */
write_word(FILE *fp, /* I - File to write to */
	   unsigned short w) { /* I - Integer to write */
    putc(w, fp);
    return (putc(w >> 8, fp));
}


/*
 * 'write_dword()' - Write a 32-bit unsigned integer.
 */
int /* O - 0 on success, -1 on error */
write_dword(FILE *fp, /* I - File to write to */
	    unsigned int dw) { /* I - Integer to write */
    putc(dw, fp);
    putc(dw >> 8, fp);
    putc(dw >> 16, fp);
    return (putc(dw >> 24, fp));
}


/*
 * 'write_long()' - Write a 32-bit signed integer.
 */
int /* O - 0 on success, -1 on error */
write_long(FILE *fp, /* I - File to write to */
	   int l) { /* I - Integer to write */
    putc(l, fp);
    putc(l >> 8, fp);
    putc(l >> 16, fp);
    return (putc(l >> 24, fp));
}


color_image_t read_BMP(const char* filename, int *imgW, int *imgH) {
    FILE *img1;
    int i, j, k, w, h, size1;
    unsigned char *pixels;
    color_image_t RGB;

    img1 = fopen(filename, "r");
    if (img1 == NULL) {
	cerr << "Cannot open " << filename << ", bailing out..." << endl;
	return NULL;
    }
    rewind(img1);

    header.bfType = read_word(img1);
    header.bfSize = read_dword(img1);
    header.bfReserved1 = read_word(img1);
    header.bfReserved2 = read_word(img1);
    header.bfOffBits = read_dword(img1);

    if (header.bfType != (('M' << 8) | 'B')) { /* Check for BM reversed... */
	/* Not a bitmap file - return NULL... */
	cerr << "Not a BMP!" << endl;
	fclose(img1);
	return NULL;
    }
    info.biSize = read_dword(img1);
    info.biWidth = read_long(img1);
    info.biHeight = read_long(img1);
    info.biPlanes = read_word(img1);
    info.biBitCount = read_word(img1);
    info.biCompression = read_dword(img1);
    info.biSizeImage = read_dword(img1);
    info.biXPelsPerMeter = read_long(img1);
    info.biYPelsPerMeter = read_long(img1);
    info.biClrUsed = read_dword(img1);
    info.biClrImportant = read_dword(img1);

    *imgH = h = info.biHeight;
    *imgW = w = info.biWidth;

    if (info.biSizeImage > 0) {
	size1 = info.biSizeImage / h;
    } else {
	size1 = (info.biWidth * info.biBitCount);
	// image lines are padded to multiple of 32 bits
	size1 += (32 - size1 % 32) % 32;
	size1 /= 8;
    }

    pixels = (unsigned char*) malloc(size1 * sizeof(unsigned char));

    RGB = (color_image_t) malloc(h * w * sizeof(color_t));

    if (info.biBitCount == 8) {
	//read the Color Table
	RGBQUAD* table = (RGBQUAD*) malloc(info.biClrUsed * sizeof(RGBQUAD));
	safeRead(table, 1, info.biClrUsed * sizeof(RGBQUAD), img1);
	fseek(img1, header.bfOffBits, SEEK_SET);

	for (i = 0; i < h; i++) {
	    safeRead(pixels, 1, size1, img1);
	    k = i * w;

	    for (j = 0; j < w; j++) {
		int color = pixels[j];
		RGB[k].B = table[color].rgbBlue;
		RGB[k].G = table[color].rgbGreen;
		RGB[k].R = table[color].rgbRed;
		k++;
	    }
	}
	free(table);
    } else if (info.biBitCount < 8) {
	// biBitCount is 1, 2 or 4

	//read the Color Table
	RGBQUAD* table = (RGBQUAD*) malloc(info.biClrUsed * sizeof(RGBQUAD));
	safeRead(table, 1, info.biClrUsed * sizeof(RGBQUAD), img1);

	fseek(img1, header.bfOffBits, SEEK_SET);

	int shiftStart;
	unsigned char mask;
	if (info.biBitCount == 1) {
	    mask = 0x01;
	    shiftStart = 7;
	} else if (info.biBitCount == 2) {
	    mask = 0x03;
	    shiftStart = 6;
	} else { // info.biBitCount == 4 (by BMP specification)
	    mask = 0x0F;
	    shiftStart = 4;
	}

	for (i = 0; i < h; i++) {
	    safeRead(pixels, 1, size1, img1);
	    k = i * w;

	    unsigned char *currentByte = pixels;
	    int shift = shiftStart;

	    for (j = 0; j < w; j++) {
		int color = (*currentByte >> shift) & mask;
		RGB[k].B = table[color].rgbBlue;
		RGB[k].G = table[color].rgbGreen;
		RGB[k].R = table[color].rgbRed;
		k++;
		shift -= info.biBitCount;
		if (shift < 0) {
		    currentByte++;
		    shift = shiftStart;
		}
	    }
	}
	free(table);
    } else if (info.biBitCount == 24) {
	fseek(img1, header.bfOffBits, SEEK_SET);
	// Probably a regular RGB bitmap
	for (i = 0; i < h; i++) {
	    safeRead(pixels, 1, size1, img1);
	    k = i * w;
	    for (j = 0; j < w; j++) {
		RGB[k].B = pixels[j * 3];
		RGB[k].G = pixels[j * 3 + 1];
		RGB[k++].R = pixels[j * 3 + 2];
	    }
	}
    } else {
	// oh boy...
    }

    fclose(img1);

    free(pixels);
    return RGB;
}


int write_BMP(const char* filename, color_image_t RGB, int imgW, int imgH) {
    FILE *img1;
    int i, j, k, size1;
    unsigned char *pixels;

    img1 = fopen(filename, "wb");
    if (img1 == NULL) {
	cerr << "Cannot open " << filename << ", bailing out..." << endl;
	return -1;
    }
    // change the header
    size1 = imgW * 3; // 3 bytes per pixel
    size1 += (4 - size1 % 4) % 4; // image lines are padded to multiple of 32 bits

    header.bfType = (('M' << 8) | 'B');
    header.bfOffBits = BITMAPFILEHEADER_SIZE + BITMAPINFOHEADER_SIZE;
    header.bfSize = (imgH * size1) + header.bfOffBits;
    header.bfReserved1 = 0;
    header.bfReserved2 = 0;

    write_word(img1, header.bfType); /* bfType */
    write_dword(img1, header.bfSize); /* bfSize */
    write_word(img1, header.bfReserved1); /* bfReserved1 */
    write_word(img1, header.bfReserved1); /* bfReserved2 */
    write_dword(img1, header.bfOffBits); /* bfOffBits */

    info.biSize = BITMAPINFOHEADER_SIZE;
    info.biWidth = imgW;
    info.biHeight = imgH;
    info.biPlanes = 1;
    info.biBitCount = 24;
    info.biCompression = 0;
    info.biSizeImage = size1 * imgH;
    info.biXPelsPerMeter = 0;
    info.biYPelsPerMeter = 0;
    info.biClrUsed = 0;
    info.biClrImportant = 0;

    write_dword(img1, info.biSize);
    write_long(img1, info.biWidth);
    write_long(img1, info.biHeight);
    write_word(img1, info.biPlanes);
    write_word(img1, info.biBitCount);
    write_dword(img1, info.biCompression);
    write_dword(img1, info.biSizeImage);
    write_long(img1, info.biXPelsPerMeter);
    write_long(img1, info.biYPelsPerMeter);
    write_dword(img1, info.biClrUsed);
    write_dword(img1, info.biClrImportant);

    pixels = (unsigned char*) calloc(size1, sizeof(unsigned char));
    for (i = 0; i < imgH; i++) {
	k = i * imgW;
	for (j = 0; j < imgW; j++) {
	    pixels[j * 3] = (unsigned char) RGB[k].B;
	    pixels[j * 3 + 1] = (unsigned char) RGB[k].G;
	    pixels[j * 3 + 2] = (unsigned char) RGB[k++].R;
	}
	fwrite(pixels, 1, size1, img1);
    }

    fclose(img1);
    return 1;
}


int write_GrayBMP(const char* filename, gray_image_t gray, int imgW, int imgH) {
    FILE *img1;
    int i, size1;
    RGBQUAD table[256];

    img1 = fopen(filename, "wb");
    if (img1 == NULL) {
	cerr << "Cannot open " << filename << ", bailing out..." << endl;
	return -1;
    }

    // image lines are padded to multiple of 32 bits
    size1 = imgW + ((4 - imgW % 4) % 4);

    header.bfType = (('M' << 8) | 'B');
    header.bfOffBits = BITMAPFILEHEADER_SIZE + BITMAPINFOHEADER_SIZE + 256
	* sizeof(RGBQUAD);
    header.bfSize = (imgH * size1) + header.bfOffBits;
    header.bfReserved1 = 0;
    header.bfReserved2 = 0;

    write_word(img1, header.bfType);
    write_dword(img1, header.bfSize);
    write_word(img1, header.bfReserved1);
    write_word(img1, header.bfReserved1);
    write_dword(img1, header.bfOffBits);

    info.biSize = BITMAPINFOHEADER_SIZE;
    info.biWidth = imgW;
    info.biHeight = imgH;
    info.biPlanes = 1;
    info.biBitCount = 8;
    info.biCompression = 0;
    info.biSizeImage = size1 * imgH;
    info.biXPelsPerMeter = 0;
    info.biYPelsPerMeter = 0;
    info.biClrUsed = 256;
    info.biClrImportant = 256;

    write_dword(img1, info.biSize);
    write_long(img1, info.biWidth);
    write_long(img1, info.biHeight);
    write_word(img1, info.biPlanes);
    write_word(img1, info.biBitCount);
    write_dword(img1, info.biCompression);
    write_dword(img1, info.biSizeImage);
    write_long(img1, info.biXPelsPerMeter);
    write_long(img1, info.biYPelsPerMeter);
    write_dword(img1, info.biClrUsed);
    write_dword(img1, info.biClrImportant);

    for (i = 0; i < 256; i++) {
	table[i].rgbRed = i;
	table[i].rgbBlue = i;
	table[i].rgbGreen = i;
	table[i].rgbReserved = 0;
    }
    fwrite(table, 1, 256 * sizeof(RGBQUAD), img1);

    unsigned char* pixels = (unsigned char*) calloc(size1,
						    sizeof(unsigned char));

    for (i = 0; i < imgH; i++) {
	for (int j = 0; j < imgW; j++) {
	    pixels[j] = (unsigned char) gray[i * imgW + j];
	}

	fwrite(pixels, 1, size1, img1);
    }

    fclose(img1);
    return 1;
}
