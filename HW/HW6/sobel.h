#ifndef SOBEL_H_
#define SOBEL_H_

#include "ap_int.h"
#include "ap_fixed.h"
#define WIDTH 	1280
#define HEIGHT	720
typedef ap_int<9> PIXEL;
void sobel(PIXEL* src, PIXEL* dst, int rows, int cols);

#endif
