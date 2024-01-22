#include <iostream>
#include "FIR.h"
using namespace std;
#define N 16
int main(int argc, char **argv)
{
   data_t int_x;
   acc_t hw_result;
   acc_t sw_result;
   int err_cnt = 0;

   coef_t coef[N] = {1,-2,3,-4,5,-6,7,-8,-8,7,-6,5,-4,3,-2,1};
   static data_t reg[N];
   acc_t accum;

   int i,j;
   
   cout << "{";
   for (j = 0; j < N; j++){
      accum = 0;
      if (j == 0){
    	  int_x = 1;
      } else {
    	  int_x = 0;
      }
      for (i = N - 1; i >= 0; i--) {
         if (i == 0) {
            accum += int_x * coef[0];
            reg[0] = int_x;
         } else {
            reg[i] = reg[i - 1];
            accum += reg[i] * coef[i];
         }
      }
      sw_result = accum;

#ifdef HW_COSIM
      // Run the AutoESL fir block
      fir(&hw_result,int_x);
#endif

#ifdef HW_COSIM
      // Print fir result 
      cout << hw_result <<" ";
      // Check HW result against SW
      if (hw_result != sw_result) {
         err_cnt++;
         cout << "*";
      }
#else
      cout << sw_result;
#endif

   }
   cout << "}" << endl;
   
#ifdef HW_COSIM
   if (err_cnt)
      cout << "ERROR: " << err_cnt << " mismatches detected!" << endl;
   else
      cout << "Test passed." << endl;
#endif
   return err_cnt;

}


