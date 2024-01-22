#include "headseeker.h"
#include <fstream>
#include <iostream>
#include <stdio.h>

void headseeker(hls::stream<ap_axiu<32,0,0,0> >& msg_in,
hls::stream<ap_axiu<32,0,0,0> >& msg_out)
{
#pragma HLS pipeline II =1
#pragma HLS INTERFACE axis port=msg_in
#pragma HLS INTERFACE axis port=msg_out

	ap_axiu<32,0,0,0> curr_word;
	ap_axiu<32,0,0,0> send_word;
	static ap_axiu<64,0,0,0> buffer_word;
	static ap_axiu<24,0,0,0> tmp_word;
	static ap_axiu<24,0,0,0> tmp_reg;
	ap_uint<32> msg;
	static ap_uint<2> flag;
	static ap_uint<6> count;
	static enum dState {D_IDLE = 0, D_SEARCH, D_FOUND } findState;

	if(!msg_in.empty()){
		msg_in.read(curr_word);
		msg = curr_word.data;
	}

	if (msg == 0x23232323){
		msg = 0x20202020;
	}

	buffer_word.data.range(63, 32) = buffer_word.data.range(31, 0);
	buffer_word.data.range(31, 0) = msg.range(31, 0);


	switch(findState){
	case D_IDLE:
		count = 0;
		tmp_word.data = 0;
		tmp_reg.data = 0;
		flag = 0;
		if (msg != 0){
			findState = D_SEARCH;
		} else {
			findState = D_IDLE;
		}

	break;

	case D_SEARCH:
		if (buffer_word.data.range(55,16) == EE216){
			flag = 3;
			count = count +1;
			tmp_word.data.range(23,0) = buffer_word.data.range(23,0);
			buffer_word.data.range(31,0) = buffer_word.data.range(55,24);
			buffer_word.data.range(55,32) = 0;
			findState = D_FOUND;
		} else if (buffer_word.data.range(47,8) == EE216){
			flag = 2;
			count = count +2;
			tmp_word.data.range(15,0) = buffer_word.data.range(15,0);
			buffer_word.data.range(31,0) = buffer_word.data.range(47,16);
			buffer_word.data.range(47,32) = 0;
			findState = D_FOUND;
		} else if (buffer_word.data.range(39,0) == EE216){
			flag = 1;
			count = count +3;
			tmp_word.data.range(7,0) = buffer_word.data.range(7,0);
			buffer_word.data.range(31,0) = buffer_word.data.range(39,8);
			buffer_word.data.range(39,32) = 0;
			findState = D_FOUND;
		} else {
			count = count + 4;
			if (count >= 32){
				findState = D_IDLE;
			} else {
				findState = D_SEARCH;
			}
		}

	break;

	case D_FOUND:
		count = count + 4;
		if (count >= 32){
			findState = D_IDLE;
		} else if (flag == 3){
			tmp_reg.data.range(23,0) = tmp_word.data.range(23,0);
			tmp_word.data.range(23,0) = buffer_word.data.range(23,0);
			buffer_word.data.range(7,0) = buffer_word.data.range(31,24);
			buffer_word.data.range(31,8) = tmp_reg.data.range(23,0);
			findState = D_FOUND;
		} else if (flag == 2){
			tmp_reg.data.range(15,0) = tmp_word.data.range(15,0);
			tmp_word.data.range(15,0) = buffer_word.data.range(15,0);
			buffer_word.data.range(15,0) = buffer_word.data.range(31,16);
			buffer_word.data.range(31,16) = tmp_reg.data.range(15,0);
			findState = D_FOUND;
		} else if (flag == 1){
			tmp_reg.data.range(7,0) = tmp_word.data.range(7,0);
			tmp_word.data.range(7,0) = buffer_word.data.range(7,0);
			buffer_word.data.range(23,0) = buffer_word.data.range(31,8);
			buffer_word.data.range(31,24) = tmp_reg.data.range(7,0);
			findState = D_FOUND;
		}

	break;
	}

	send_word.data = buffer_word.data.range(63,32);
	msg_out.write(send_word);
}
