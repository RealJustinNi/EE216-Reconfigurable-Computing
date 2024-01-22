#ifndef SOBEL_H_
#define SOBEL_H_
#include "ap_int.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"
using namespace hls;
#define WIDTH 	1280
#define HEIGHT	720
#define WIDTH_OUT 1278
#define HEIGHT_OUT 718

typedef unsigned char PIXEL;
typedef ap_int<11> TMP;
typedef ap_uint<9> SUM;
int sobel(
		stream<ap_axiu<8,1,1,1>>& src,
		stream<ap_axiu<8,1,1,1>>& dst,
		int rows,
		int cols
);

#endif
