#include "FIR.h"
#define N 16

void fir(data_t *y, data_t x) {
  coef_t c[N] = {1,-2,3,-4,5,-6,7,-8,-8,7,-6,5,-4,3,-2,1};
  static data_t shift_reg[N];
  acc_t acc;
  int i;
  SHIFT:
	  for (i = N - 1; i > 0; i--) {
		  shift_reg[i] = shift_reg[i - 1];
	  }
  shift_reg[0] = x;
  acc = 0;

#pragma HLS pipeline II = 1
  MAC:
  for (i = N - 1; i >= 0; i--) {
#pragma HLS unroll factor = 16
	  acc += shift_reg[i] * c[i];
  }
  *y = acc;
}
