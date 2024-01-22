#ifndef __FIR_H__
#define __FIR_H__

using namespace std;

typedef int coef_t;
typedef int data_t;
typedef int acc_t;


// Prototype of top level function for C-synthesis
void fir(
		data_t *y, data_t x);

#endif // __FIR_H__ not defined
