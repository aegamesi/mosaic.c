#include "lodepng.h"

#include <stdio.h>
#include <stdlib.h>

unsigned char* in_image;
unsigned char* out_image;
unsigned width, height, error;

int get_pixel(int x, int y, int o) {
	return (int) in_image[4 * y * width + 4 * x + o];
}

void put_pixel(int x, int y, unsigned int c) {
	out_image[4 * y * width + 4 * x + 0] = c;
	out_image[4 * y * width + 4 * x + 1] = c;
	out_image[4 * y * width + 4 * x + 2] = c;
	out_image[4 * y * width + 4 * x + 3] = 255;
}

void print_usage() {
	printf("mosaic.c -- mEli Lipsitz\n");
	printf("Usage: mosaic <file> <out file>\n");
	printf("<file> must be in PNG format.\n");
}

int isPowerOfTwo(unsigned int x) {
  return ((x != 0) && !(x & (x - 1)));
}

// denero functions
void image_paste(int left, int top, int w, int h, int tint) {
	for(int x = left; x < left + w; x++) {
		for(int y = top; y < top + h; y++) {
			int i = get_pixel(((x - left) * width) / w, ((y - top) * height) / h, 0);
			i = i / 3 + 85; // contrast (x3)
			i = (i * tint * 4) / (255 * 3); // boost brightness too
			put_pixel(x, y, i);
		}
	}
}

int image_average(int left, int top, int w, int h) {
	long long int total = 0;
	for(int x = left; x < left + w; x++) {
		for(int y = top; y < top + h; y++) {
			total += get_pixel(x, y, 0);
		}
	}
	return total / (w * h);
}

int image_squares(int left, int top, int w, int h, int avg) {
	long long int total = 0;
	for(int x = left; x < left + w; x++) {
		for(int y = top; y < top + h; y++) {
			int pix = (avg - get_pixel(x, y, 0));
			total += pix * pix;
		}
	}
	return total / (w * h);
}

void image_mosaic(int left, int top, int w, int h) {
	int avg = image_average(left, top, w, h);
	int s = image_squares(left, top, w, h, avg);

	if(s < 64 || w <= 8) {
		image_paste(left, top, w, h, avg);
	} else {
		w = w / 2;
		h = h / 2;
		image_mosaic(left, top, w, h);
		image_mosaic(left + w, top, w, h);
		image_mosaic(left, top + h, w, h);
		image_mosaic(left + w, top + h, w, h);
	}
}
// end denero functions

int main(int argc, char* argv[])  {
	if(argc != 3) {
		print_usage();
		return 0;
	}
	
	char* filename_input = argv[1];
	char* filename_output = argv[2];

	// read image
	error = lodepng_decode32_file(&in_image, &width, &height, filename_input);
	if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
	out_image = malloc(width * height * 4);

	// check the image
	if(!isPowerOfTwo(width) || !isPowerOfTwo(height)) {
		printf("Error: image dimensions must be PoT\n");
		return -1;
	}
	if(width != height) {
		printf("Error: image must be square\n");
		return -1;
	}
	
	// do the image
	image_mosaic(0, 0, width, height);

	// write image
	error = lodepng_encode32_file(filename_output, out_image, width, height);
	if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

	// free data
	free(in_image);
	free(out_image);
}