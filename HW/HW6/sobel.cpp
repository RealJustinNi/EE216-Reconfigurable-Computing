#include "sobel.h"

static PIXEL Gradient_X(PIXEL WB[3][3]){
	return WB[0][2] + 2 * WB[1][2] + WB[2][2] - (WB[0][0] + 2 * WB[1][0] + WB[2][0]);
}

static PIXEL Gradient_Y(PIXEL WB[3][3]){
	return WB[2][0] + 2 * WB[2][1] + WB[2][2] - (WB[0][0] + 2 * WB[0][1] + WB[0][2]);
}

static PIXEL sobel3x3_kernel(PIXEL WB[3][3]){
	PIXEL gradient_x = Gradient_X(WB);
	PIXEL gradient_y = Gradient_Y(WB);
	PIXEL total_gradient = gradient_x + gradient_y;

	total_gradient = (total_gradient < (PIXEL)0) ? (PIXEL)0 : total_gradient;
	total_gradient = (total_gradient > (PIXEL)255) ? (PIXEL)255 : total_gradient;

	return total_gradient;
}

void sobel(PIXEL src[HEIGHT*WIDTH], PIXEL dst[(HEIGHT-2)*(WIDTH-2)], int rows, int cols){
	PIXEL window[3][3];
	PIXEL line_buffer[2][WIDTH];
	#pragma HLS array_partition variable=line_buffer complete dim=1

	ROW_LOOP:
	for (int i = 0; i < rows; i++){
	#pragma HLS loop_tripcount min=720 max=720
		COL_LOOP:
		for (int j = 0; j < cols; j++){
		#pragma HLS pipeline II = 1
		#pragma HLS loop_tripcount min=1280 max=1280
		for (ap_uint<2> k = 0; k < 3; k++){
			#pragma HLS unroll factor=3
			window[k][0] = window[k][1];
			window[k][1] = window[k][2];
		}
		window[0][2] 		= line_buffer[0][j];
		window[1][2] 		= line_buffer[1][j];
		line_buffer[0][j]	= line_buffer[1][j];
		window[2][2]		= src[i * cols + j];
		line_buffer[1][j]   = src[i * cols + j];

		if (i >= 2 && j >= 2){
			dst[(i - 2) * (cols - 2) + j - 2] = sobel3x3_kernel(window);
		}

		}
	}
}
