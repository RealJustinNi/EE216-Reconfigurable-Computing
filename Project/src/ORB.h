#ifndef _ORB_H_
#define _ORB_H_

#include "hls_stream.h"
#include "hls_math.h"
#include "common/xf_common.hpp"
#include "common/xf_video_mem.hpp"
#include "imgproc/xf_resize.hpp"
#include "fast.hpp"
#include "pattern.h"

#define ANGLE_WIDTH 10
#define WIND_SIZE 37
#define HALF_WIND_SIZE 18
#define M_WIDTH 20
#define NMS 1
#define DEPTH 480*640

typedef struct tagWindowCol {
    ap_uint<1> flag_last;
    ap_uint<1> flag_begin;
    ap_uint<1> flag_end;
    ap_uint<16> x;
    ap_uint<16> y;
    ap_uint<8*WIND_SIZE> col_img;
    ap_uint<8*WIND_SIZE> col_blur;
    ap_uint<8> response;
} WindowCol;

typedef struct tagDescOut {
    ap_uint<1> valid;
    ap_uint<16> x;
    ap_uint<16> y;
    ap_uint<8> response;
    ap_uint<256> descriptor;
} DescOut;

int ORB_accel(
    hls::stream<ap_axiu<8, 1, 1, 1>>& src,
    hls::stream<ap_axiu<16*2+8+256, 1, 1, 1>>& dst,
    int height,
    int width,
    int height_new,
    int width_new,
    int threshold
);

#endif