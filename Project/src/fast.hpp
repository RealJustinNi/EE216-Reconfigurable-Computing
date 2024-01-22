#include "hls_stream.h"
#include "common/xf_common.hpp"
#include "common/xf_utility.hpp"

#define __MIN(a, b) ((a < b) ? a : b)
#define __MAX(a, b) ((a > b) ? a : b)

static ap_uint<8> fast_score(int* flag_d, int _threshold) {
    short int flag_d_min2[25 - 1];
#pragma HLS ARRAY_PARTITION variable=flag_d_min2 dim=1 complete
    short int flag_d_max2[25 - 1];
#pragma HLS ARRAY_PARTITION variable=flag_d_max2 dim=1 complete
    short int flag_d_min4[25 - 3];
#pragma HLS ARRAY_PARTITION variable=flag_d_min4 dim=1 complete
    short int flag_d_max4[25 - 3];
#pragma HLS ARRAY_PARTITION variable=flag_d_max4 dim=1 complete
    short int flag_d_min8[25 - 7];
#pragma HLS ARRAY_PARTITION variable=flag_d_min8 dim=1 complete
    short int flag_d_max8[25 - 7];
#pragma HLS ARRAY_PARTITION variable=flag_d_max8 dim=1 complete

    for (ap_uint<5> i = 0; i < 25 - 1; i++) {
#pragma HLS UNROLL factor=24
        flag_d_min2[i] = __MIN(flag_d[i], flag_d[i + 1]);
        flag_d_max2[i] = __MAX(flag_d[i], flag_d[i + 1]);
    }

    for (ap_uint<5> i = 0; i < 25 - 3; i++) {
#pragma HLS UNROLL factor=22
        flag_d_min4[i] = __MIN(flag_d_min2[i], flag_d_min2[i + 2]);
        flag_d_max4[i] = __MAX(flag_d_max2[i], flag_d_max2[i + 2]);
    }

    for (ap_uint<5> i = 0; i < 25 - 7; i++) {
#pragma HLS UNROLL factor=18
        flag_d_min8[i] = __MIN(flag_d_min4[i], flag_d_min4[i + 4]);
        flag_d_max8[i] = __MAX(flag_d_max4[i], flag_d_max4[i + 4]);
    }

    uchar_t a0 = _threshold;

    for (ap_uint<5> i = 0; i < 16; i += 2) {
#pragma HLS UNROLL factor=8
        short int a = 255;
        a = flag_d_min8[i + 1];
        a0 = __MAX(a0, __MIN(a, flag_d[i])); // a0 >= _threshold
        a0 = __MAX(a0, __MIN(a, flag_d[i + 16 / 2 + 1]));
    }
    short int b0 = -_threshold;
    for (ap_uint<5> i = 0; i < 16; i += 2) {
#pragma HLS UNROLL factor=8
        short int b = -255;
        b = flag_d_max8[i + 1];
        b0 = __MIN(b0, __MAX(b, flag_d[i])); // b0 <= -_threshold
        b0 = __MIN(b0, __MAX(b, flag_d[i + 16 / 2 + 1]));
    }
    ap_uint<8> score = __MAX(a0, -b0) - 1;
    return score;
}

template<int W, int HEIGHT, int WIDTH, int NPC>
void process_fast(
    hls::stream<ap_uint<W>>& img_in,
    hls::stream<ap_uint<W>>& img_out,
    hls::stream<ap_uint<W>>& blur_in,
    hls::stream<ap_uint<W>>& blur_out,
    hls::stream<ap_uint<W>>& mask_out,
    int height,
    int width,
    unsigned char _threshold
) {
    xf::cv::LineBuffer<7, WIDTH, ap_uint<W>> line_buffer;
    xf::cv::Window<7, 7, ap_uint<W>> window;
    ap_uint<W> pixel, pixel_blur, mask, score;
    int flag[25];
#pragma HLS ARRAY_PARTITION variable=flag dim=1 complete
    int diff[25];
#pragma HLS ARRAY_PARTITION variable=diff dim=1 complete
    int cnt = 0;
    int write_cnt = 0;

    loop_buffer_row:
    for (int i = 0; i < height; i++) {
#pragma HLS LOOP_TRIPCOUNT max=1080 min=1080
        loop_buffer_col:
        for (int j = 0; j < width; j++) {
#pragma HLS LOOP_TRIPCOUNT max=1920 min=1920
            pixel = img_in.read();
            pixel_blur = blur_in.read();
            blur_out.write(pixel_blur);
            img_out.write(pixel);
            line_buffer.shift_pixels_up(j);
            line_buffer.insert_bottom_row(pixel, j);
            window.shift_pixels_left();
            for (int k = 0; k < 7; k++) {
#pragma HLS UNROLL factor=7
                window.insert_pixel(line_buffer(k, j), k, 6);
            }
            if (cnt >= 3*width + 3) {
                mask = 0;
                if (i >= 6 && j >= 6) {
                    diff[0] =  window(3, 3) - window(0, 3);
                    diff[1] =  window(3, 3) - window(0, 4);
                    diff[2] =  window(3, 3) - window(1, 5);
                    diff[3] =  window(3, 3) - window(2, 6);
                    diff[4] =  window(3, 3) - window(3, 6);
                    diff[5] =  window(3, 3) - window(4, 6);
                    diff[6] =  window(3, 3) - window(5, 5);
                    diff[7] =  window(3, 3) - window(6, 4);
                    diff[8] =  window(3, 3) - window(6, 3);
                    diff[9] =  window(3, 3) - window(6, 2);
                    diff[10] = window(3, 3) - window(5, 1);
                    diff[11] = window(3, 3) - window(4, 0);
                    diff[12] = window(3, 3) - window(3, 0);
                    diff[13] = window(3, 3) - window(2, 0);
                    diff[14] = window(3, 3) - window(1, 1);
                    diff[15] = window(3, 3) - window(0, 2);
                    diff[16] = diff[0];
                    diff[17] = diff[1];
                    diff[18] = diff[2];
                    diff[19] = diff[3];
                    diff[20] = diff[4];
                    diff[21] = diff[5];
                    diff[22] = diff[6];
                    diff[23] = diff[7];
                    diff[24] = diff[8];

                    for (int n = 0; n < 16; n++) {
#pragma HLS UNROLL factor=16
                        if (diff[n] > _threshold)
                            flag[n] = 1;
                        else if (diff[n] < -_threshold)
                            flag[n] = 2;
                        else
                            flag[n] = 0;
                    }
                    flag[16] = flag[0];
                    flag[17] = flag[1];
                    flag[18] = flag[2];
                    flag[19] = flag[3];
                    flag[20] = flag[4];
                    flag[21] = flag[5];
                    flag[22] = flag[6];
                    flag[23] = flag[7];
                    flag[24] = flag[8];

                    int cnt_p = 0;
                    int is_corner = 0;
                    for (int c = 1; c < 25; c++) {
#pragma HLS UNROLL factor=24
                        if ((flag[c - 1] == flag[c]) && flag[c] > 0) {
                            cnt_p++;
                            if (cnt_p > 8) {
                                is_corner = 1;
                            }
                        } else {
                            cnt_p = 1;
                        }
                    }
                    if (is_corner) {
                        mask = fast_score(diff, _threshold);
                    }  
                }
                mask_out.write(mask);
                write_cnt++;
            }
            cnt++;
        }
    }
    for (int k = 0; k < width*3+3; k++) {
#pragma HLS LOOP_TRIPCOUNT max=5763 min=5763
//#pragma HLS UNROLL factor=5763
        mask_out.write(0);
    }
}

template<int W, int HEIGHT, int WIDTH, int NPC>
void process_nms(
    hls::stream<ap_uint<W>>& img_in,
    hls::stream<ap_uint<W>>& img_out,
    hls::stream<ap_uint<W>>& blur_in,
    hls::stream<ap_uint<W>>& blur_out,
    hls::stream<ap_uint<W>>& mask_in,
    hls::stream<ap_uint<W>>& mask_out,
    int height,
    int width
) {
    ap_uint<8> pix;
    xf::cv::LineBuffer<3, WIDTH, ap_uint<W>> line_buffer;
    xf::cv::Window<3, 3, ap_uint<W>> window;
    ap_uint<W> pixel, pixel_blur, mask, mask_new;
    int cnt = 0;
    int write_cnt = 0;

    loop_buffer_row:
    for (int i = 0; i < height; i++) {
#pragma HLS LOOP_TRIPCOUNT max=1080 min=1080
        loop_buffer_col:
        for (int j = 0; j < width; j++) {
#pragma HLS LOOP_TRIPCOUNT max=1920 min=1920
            pixel = img_in.read();
            mask = mask_in.read();
            pixel_blur = blur_in.read();
            blur_out.write(pixel_blur);
            img_out.write(pixel);
            line_buffer.shift_pixels_up(j);
            line_buffer.insert_bottom_row(mask, j);
            window.shift_pixels_left();
            for (int k = 0; k < 3; k++) {
#pragma HLS UNROLL factor=3
                window.insert_pixel(line_buffer(k, j), k, 3-1);
            }
            if (cnt >= 1*width + 1) {
                if (i >= 3-1 && j >= 3-1) {
                    if (window(1, 1) != 0) { // if score of candidate pixel != 0
                        if ((window(1, 1) > window(1, 0)) && (window(1, 1) > window(1, 2)) && (window(1, 1) > window(0, 0)) &&
                            (window(1, 1) > window(0, 1)) && (window(1, 1) > window(0, 2)) && (window(1, 1) > window(2, 0)) &&
                            (window(1, 1) > window(2, 1)) && (window(1, 1) > window(2, 2))) {
                            mask_new = window(1, 1);
                        } else {
                            mask_new = 0;
                        }
                    } else {
                        mask_new = 0;
                    }
                } else {
                    mask_new = 0;
                }
                mask_out.write(mask_new);
                write_cnt++;
            }
            cnt++;
        }
    }
    for (int k = 0; k < width*1+1; k++) {
//#pragma HLS UNROLL factor=1921
#pragma HLS LOOP_TRIPCOUNT max=1921 min=1921
        mask_out.write(0);
    }
}

