#include "sobel.h"

static PIXEL Gradient_X(PIXEL WB[3][3]){
	TMP tmp =  WB[0][2] + 2 * WB[1][2] + WB[2][2] - (WB[0][0] + 2 * WB[1][0] + WB[2][0]);
	return (tmp.range(10,10))?0:(tmp>255)?255:tmp.range(7,0);
}

static PIXEL Gradient_Y(PIXEL WB[3][3]){
	TMP tmp = WB[2][0] + 2 * WB[2][1] + WB[2][2] - (WB[0][0] + 2 * WB[0][1] + WB[0][2]);
	return (tmp.range(10,10))?0:(tmp>255)?255:tmp.range(7,0);
}

static PIXEL sobel3x3_kernel(PIXEL WB[3][3]){
	PIXEL gradient_x = Gradient_X(WB);
	PIXEL gradient_y = Gradient_Y(WB);
	SUM total_gradient = gradient_x + gradient_y;
	return (total_gradient.range(8,8))?255:total_gradient.range(7,0);
}


int sobel(
		stream<ap_axiu<8,1,1,1>> &src,
		stream<ap_axiu<8,1,1,1>> &dst,
		int rows,
		int cols
){
#pragma HLS INTERFACE axis port=src depth=1024
#pragma HLS INTERFACE axis port=dst depth=1024
#pragma HLS INTERFACE s_axilite port=rows
#pragma HLS INTERFACE s_axilite port=cols
#pragma HLS INTERFACE s_axilite port=return
	ap_axiu<8, 1, 1, 1> axis_in;
	ap_axiu<8, 1, 1, 1> axis_out;
	PIXEL src_in;
	PIXEL dst_out;
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
			axis_in = src.read();
			src_in = axis_in.data;
			window[2][2]		= src_in;
			line_buffer[1][j]   = src_in;

			if (i >= 2 && j >= 2){
				dst_out = sobel3x3_kernel(window);
				axis_out.data = dst_out;
				axis_out.keep = -1;
				if ((i - 2) * (cols - 2) + j - 2 == (rows-2)*(cols-2)-1){
					axis_out.last = 1;
				} else {
					axis_out.last = 0;
				}
				dst.write(axis_out);
			}

		}
	}
	return 0;
}
