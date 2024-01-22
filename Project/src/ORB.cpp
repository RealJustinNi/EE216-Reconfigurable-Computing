#include "ORB.h"

static int idxTree[256][100] = {0};
static int cntTree[256] = {0};

ap_fixed<ANGLE_WIDTH, 3> IC_Angle_31(xf::cv::Window<WIND_SIZE, WIND_SIZE, ap_uint<8>>& window){
    ap_int<M_WIDTH> m_01, m_10, val_plus, val_minus, v_sum;
    ap_fixed<ANGLE_WIDTH, 3> ret;

    m_01 = 0;
    m_10 = 0;
    for (int u = -15; u <= 15; ++u) {
#pragma HLS UNROLL factor=31
        m_10 += u * window(15, 15 + u);
    }
    for (int v = 1; v <= 3; ++v) {
#pragma HLS UNROLL factor=3
		v_sum = 0;
		for (int u = -15; u <= 15; ++u) {
#pragma HLS UNROLL factor=31
			val_plus = window(15+v, 15+u);
			val_minus = window(15-v, 15+u);
			v_sum += (val_plus - val_minus);
			m_10 += u * (val_plus + val_minus);
		}
		m_01 += v * v_sum;
	}
	for (int v = 4; v <= 6; ++v) {
#pragma HLS UNROLL factor=3
		v_sum = 0;
		for (int u = -14; u <= 14; ++u) {
#pragma HLS UNROLL factor=29
			val_plus = window(15+v, 15+u);
			val_minus = window(15-v, 15+u);
			v_sum += (val_plus - val_minus);
			m_10 += u * (val_plus + val_minus);
		}
		m_01 += v * v_sum;
	}
	for (int v = 7; v <= 8; ++v) {
#pragma HLS UNROLL factor=2
	  v_sum = 0;
	  for (int u = -13; u <= 13; ++u) {
#pragma HLS UNROLL factor=27
		  val_plus = window(15+v, 15+u);
		  val_minus = window(15-v, 15+u);
		  v_sum += (val_plus - val_minus);
		  m_10 += u * (val_plus + val_minus);
	  }
	  m_01 += v * v_sum;
	}

	v_sum = 0;
	for (int u = -12; u <= 12; ++u) {
#pragma HLS UNROLL factor=25
	  val_plus = window(15+9, 15+u);
	  val_minus = window(15-9, 15+u);
	  v_sum += (val_plus - val_minus);
	  m_10 += u * (val_plus + val_minus);
	}
	m_01 += 9 * v_sum;

	v_sum = 0;
	for (int u = -11; u <= 11; ++u) {
#pragma HLS UNROLL factor=23
	  val_plus = window(15+10, 15+u);
	  val_minus = window(15-10, 15+u);
	  v_sum += (val_plus - val_minus);
	  m_10 += u * (val_plus + val_minus);
	}
	m_01 += 10 * v_sum;

	v_sum = 0;
	for (int u = -10; u <= 10; ++u) {
#pragma HLS UNROLL factor=21
	  val_plus = window(15+11, 15+u);
	  val_minus = window(15-11, 15+u);
	  v_sum += (val_plus - val_minus);
	  m_10 += u * (val_plus + val_minus);
	}
	m_01 += 11 * v_sum;

	v_sum = 0;
	for (int u = -9; u <= 9; ++u) {
#pragma HLS UNROLL factor=19
	  val_plus = window(15+12, 15+u);
	  val_minus = window(15-12, 15+u);
	  v_sum += (val_plus - val_minus);
	  m_10 += u * (val_plus + val_minus);
	}
	m_01 += 12 * v_sum;

	v_sum = 0;
	for (int u = -8; u <= 8; ++u) {
#pragma HLS UNROLL factor=17
	  val_plus = window(15+13, 15+u);
	  val_minus = window(15-13, 15+u);
	  v_sum += (val_plus - val_minus);
	  m_10 += u * (val_plus + val_minus);
	}
	m_01 += 13 * v_sum;

	v_sum = 0;
	for (int u = -6; u <= 6; ++u) {
#pragma HLS UNROLL factor=13
	  val_plus = window(15+14, 15+u);
	  val_minus = window(15-14, 15+u);
	  v_sum += (val_plus - val_minus);
	  m_10 += u * (val_plus + val_minus);
	}
	m_01 += 14 * v_sum;

	v_sum = 0;
	for (int u = -3; u <= 3; ++u) {
#pragma HLS UNROLL factor=7
	  val_plus = window(15+15, 15+u);
	  val_minus = window(15-15, 15+u);
	  v_sum += (val_plus - val_minus);
	  m_10 += u * (val_plus + val_minus);
	}
	m_01 += 15 * v_sum;

    ap_fixed<M_WIDTH, M_WIDTH> m_01_fixed = m_01;
    ap_fixed<M_WIDTH, M_WIDTH> m_10_fixed = m_10;
    ret = hls::atan2<M_WIDTH, M_WIDTH>(m_01_fixed, m_10_fixed);
    return ret;
}

ap_uint<256> descriptor(xf::cv::Window<WIND_SIZE, WIND_SIZE, ap_uint<8>>& window, ap_fixed<ANGLE_WIDTH, 3> angle) {

    ap_uint<8> t0, t1;
    ap_uint<256> desc;
    ap_int<6> x0, x1, y0, y1, x0_rotated, y0_rotated, x1_rotated, y1_rotated;
    ap_fixed<ANGLE_WIDTH-3+2, 2> cosine, sine;

    cosine = hls::cos<ANGLE_WIDTH, 3>(angle);
    sine = hls::sin<ANGLE_WIDTH, 3>(angle);

#pragma HLS ARRAY_PARTITION variable=bit_pattern_31 dim=1 factor=4 cyclic

    static ap_uint<8> window_buf0[37][37]={0};
#pragma HLS ARRAY_PARTITION variable=window_buf0 dim=2 complete
   static ap_uint<8> window_buf1[37][37]={0};
#pragma HLS ARRAY_PARTITION variable=window_buf1 dim=2 complete

    buf_row_loop:
	for(int i = 0;i<37;i++){
#pragma HLS UNROLL factor=37
		buf_col_loop:
		for (int j = 0;j<37;j++){
#pragma HLS UNROLL factor=37
			window_buf0[i][j] = window(i,j);
			window_buf1[i][j] = window(i,j);
		}
	}

    loop_descriptor:
    for (int i = 0; i < 256; i++) {
//#pragma HLS UNROLL factor=16
        x0 = bit_pattern_31[i*4];
        y0 = bit_pattern_31[i*4 + 1];
        x1 = bit_pattern_31[i*4 + 2];
        y1 = bit_pattern_31[i*4 + 3];
        x0_rotated = hls::round(x0*cosine-y0*sine);
        y0_rotated = hls::round(x0*sine+y0*cosine);
        x1_rotated = hls::round(x1*cosine-y1*sine);
        y1_rotated = hls::round(x1*sine+y1*cosine);
        //t0 = window(HALF_WIND_SIZE + y0_rotated, HALF_WIND_SIZE + x0_rotated);
        //t1 = window(HALF_WIND_SIZE + y1_rotated, HALF_WIND_SIZE + x1_rotated);
        t0 = window_buf0[HALF_WIND_SIZE + y0_rotated][HALF_WIND_SIZE + x0_rotated];
        t1 = window_buf1[HALF_WIND_SIZE + y1_rotated][HALF_WIND_SIZE + x1_rotated];
        desc.set_bit(i, (t0 < t1));
    }
    return desc;
}

template<int W, int TYPE, int HEIGHT, int WIDTH, int NPC>
void process_input(
    hls::stream<ap_axiu<W, 1, 1, 1>>& src,
    hls::stream<ap_uint<W>>& dst,
    int height,
    int width
) {
    for (int i = 0; i < height*width; i++) {
#pragma HLS LOOP_TRIPCOUNT max=2073600 min=2073600
        dst.write(src.read().data);
    }
}

template<int W, int TYPE, int HEIGHT, int WIDTH, int NPC>
void process_stream2xfMat(
    hls::stream<ap_uint<W>>& src,
    xf::cv::Mat<TYPE, HEIGHT, WIDTH, NPC>& dst
) {
    for (int i = 0; i < dst.cols * dst.rows; i++) {
#pragma HLS LOOP_TRIPCOUNT max=2073600 min=2073600
        dst.write(i, src.read());
    }
}

template<int W, int TYPE, int HEIGHT, int WIDTH, int NPC>
void process_xfMat2stream(
    xf::cv::Mat<TYPE, HEIGHT, WIDTH, NPC>& src,
    hls::stream<ap_uint<W>>& dst
) {
    for (int i = 0; i < src.cols * src.rows; i++) {
#pragma HLS LOOP_TRIPCOUNT max=2073600 min=2073600
        dst.write(src.read(i));
    }
}

template<int W, int TYPE, int HEIGHT, int WIDTH, int NPC>
void process_resize(
    xf::cv::Mat<TYPE, HEIGHT, WIDTH, NPC>& src,
    xf::cv::Mat<TYPE, HEIGHT, WIDTH, NPC>& dst
) {
    xf::cv::resize<
        XF_INTERPOLATION_BILINEAR,
        TYPE,
        HEIGHT,
        WIDTH,
        HEIGHT,
        WIDTH,
        NPC,
        8
    >(src, dst);
}

template<int W, int TYPE, int HEIGHT, int WIDTH, int NPC>
void process_blur(
    hls::stream<ap_uint<W>>& img_in,
    hls::stream<ap_uint<W>>& img_out,
    hls::stream<ap_uint<W>>& blur_out,
    int height, 
    int width
) {
    xf::cv::LineBuffer<7, WIDTH, ap_uint<8>> line_buffer;
    xf::cv::Window<7, 7, ap_uint<8>> window_buffer, window;
    ap_uint<W> pixel_rd, pixel_wr;
    ap_uint<16> acc;
    int read_idx = 0;
    int write_idx = 0;
    static ap_fixed<16, 2> kernel[7*7] = {
        0.00492233, 0.00919613, 0.01338028, 0.01516185, 0.01338028, 0.00919613, 0.00492233, 
        0.00919613, 0.01718062, 0.02499766, 0.02832606, 0.02499766, 0.01718062, 0.00919613, 
        0.01338028, 0.02499766, 0.03637138, 0.04121417, 0.03637138, 0.02499766, 0.01338028, 
        0.01516185, 0.02832606, 0.04121417, 0.04670178, 0.04121417, 0.02832606, 0.01516185, 
        0.01338028, 0.02499766, 0.03637138, 0.04121417, 0.03637138, 0.02499766, 0.01338028, 
        0.00919613, 0.01718062, 0.02499766, 0.02832606, 0.02499766, 0.01718062, 0.00919613, 
        0.00492233, 0.00919613, 0.01338028, 0.01516185, 0.01338028, 0.00919613, 0.00492233
    };
#pragma HLS ARRAY_PARTITION variable=kernel dim=1 complete


    loop_init_window_row:
    for (int i = 0; i < 7; i++) {
#pragma HLS UNROLL factor=7
        loop_init_window_col:
        for (int j = 0; j < 7; j++) {
#pragma HLS UNROLL factor=7
            window_buffer.insert_pixel(0, i, j);
            window.insert_pixel(0, i, j);
        }
    }
    

    loop_init_line:
    for (int j = 0 ; j < width; j++) {
#pragma HLS LOOP_TRIPCOUNT max=1920 min=1920
        for (int i = 0; i < 7; i++) {
#pragma HLS UNROLL factor=7
            line_buffer.val[i][j] = 0;
        } 
    }

    loop_blur_row:
	for (int i=0;i<(height+4);i++){
#pragma HLS LOOP_TRIPCOUNT max=1084 min=1084
		for(int j=0;j<width;j++){
#pragma HLS LOOP_TRIPCOUNT max=1920 min=1920
	        if (i < height) {
	            pixel_rd = img_in.read();
	            img_out.write(pixel_rd);
	            read_idx++;
	        } else {
	            pixel_rd = 0;
	        }
	        line_buffer.shift_pixels_up(j);
	        line_buffer.insert_bottom_row(pixel_rd, j);
	        window_buffer.shift_pixels_left();
	        window.shift_pixels_left();
	        for (int k = 0; k < 7; k++) {
	#pragma HLS UNROLL factor=7
	            window_buffer.insert_pixel(line_buffer(k, j), k, 6);
	            if (j == 3) {
	                for (int jj = 0; jj < 3; jj++) {
	#pragma HLS UNROLL factor=3
	                    window.insert_pixel(0, k, jj);
	                }
	                for (int jj = 3; jj < 7; jj++) {
	#pragma HLS UNROLL factor=4
	                    window.insert_pixel(window_buffer(k, jj), k, jj);
	                }
	            } else if (j > 3) {
	                window.insert_pixel(line_buffer(k, j), k, 6);
	            } else {
	                window.insert_pixel(0, k, 6);
	            }
	        }
	        if (read_idx > 3*width + 3 && write_idx < width*height) {
	            acc = 0;
	            for (int i = 0; i < 7; i++) {
	#pragma HLS UNROLL factor=7
	                for (int j = 0; j < 7; j++) {
	#pragma HLS UNROLL factor=7
	                    acc += window(i, j)*kernel[i*7+j];
	                }
	            }
	            pixel_wr = acc.range(W-1, 0);
	            blur_out.write(pixel_wr);
	            write_idx++;
	        }
	    }
		}
	}





template<int W, int HEIGHT, int WIDTH>
void process_rBRIEF(
    hls::stream<ap_uint<W>>& img_in,
    hls::stream<ap_uint<W>>& img_blur,
    hls::stream<ap_uint<W>>& mask_in,
    hls::stream<DescOut>& desc_out,
    int height,
    int width
) {
    ap_uint<W> img_pixel, blur_pixel, mask, response;

    xf::cv::LineBuffer<WIND_SIZE, WIDTH, ap_uint<W>> img_line, blur_line;
    xf::cv::Window<WIND_SIZE, WIND_SIZE, ap_uint<W>> img_window, blur_window;
    xf::cv::LineBuffer<WIND_SIZE, WIDTH, ap_uint<W>> mask_line;
    xf::cv::Window<WIND_SIZE, WIND_SIZE, ap_uint<W>> mask_window;

    ap_fixed<ANGLE_WIDTH, 3> angle;
    DescOut data_write;

    loop_row:
    for (int i = 0; i < height; i++) {
#pragma HLS LOOP_TRIPCOUNT max=1080 min=1080
        loop_col:
        for (int j = 0; j < width; j++) {
#pragma HLS LOOP_TRIPCOUNT max=1920 min=1920

            mask = mask_in.read();
            img_pixel = img_in.read();
            blur_pixel = img_blur.read();

            img_line.shift_pixels_up(j);
            img_line.insert_bottom_row(img_pixel, j);
            blur_line.shift_pixels_up(j);
            blur_line.insert_bottom_row(blur_pixel, j);
            mask_line.shift_pixels_up(j);
            mask_line.insert_bottom_row(mask, j);

            img_window.shift_pixels_left();
            blur_window.shift_pixels_left();
            mask_window.shift_pixels_left();

            for (int k = 0; k < 37; k++) {
#pragma HLS UNROLL factor=37
#pragma HLS LOOP_TRIPCOUNT max=37 min=37
                img_window.insert_pixel(img_line(k, j), k, WIND_SIZE-1);
                blur_window.insert_pixel(blur_line(k, j), k, WIND_SIZE-1);
                mask_window.insert_pixel(mask_line(k, j), k, WIND_SIZE-1);
            }

            if (i >= WIND_SIZE-1 && j >= WIND_SIZE-1 && mask_window(HALF_WIND_SIZE, HALF_WIND_SIZE)) {
                data_write.x = j - HALF_WIND_SIZE;
                data_write.y = i - HALF_WIND_SIZE;
                data_write.valid = 1;
                data_write.response = mask_window(HALF_WIND_SIZE, 0);
                angle = IC_Angle_31(img_window);
                data_write.descriptor = descriptor(blur_window, angle);
                desc_out.write(data_write); 
            }
        }
    }
    data_write.valid = 0;
    desc_out.write(data_write);
}


template<int W, int HEIGHT, int WIDTH>
void process_rBRIEF_col_sel(
    hls::stream<ap_uint<W>>& img_in,
    hls::stream<ap_uint<W>>& img_blur,
    hls::stream<ap_uint<W>>& mask_in,
    hls::stream<WindowCol>& window_col,
    int height,
    int width
) {
    ap_uint<W> img_pixel, blur_pixel, mask;

    xf::cv::LineBuffer<WIND_SIZE, WIDTH, ap_uint<W>> img_line, blur_line;
    xf::cv::Window<WIND_SIZE, WIND_SIZE, ap_uint<W>> img_window, blur_window;
    xf::cv::LineBuffer<WIND_SIZE, WIDTH, ap_uint<W>> mask_line;
    xf::cv::Window<WIND_SIZE, WIND_SIZE, ap_uint<W>> mask_window;

    WindowCol data_write;
    int N = 37;

    loop_row:
    for (int i = 0; i < height; i++) {
#pragma HLS LOOP_TRIPCOUNT max=1080 min=1080
        loop_col:
        for (int j = 0; j < (width+2*HALF_WIND_SIZE); j++) {
#pragma HLS LOOP_TRIPCOUNT max=1938 min=1938

        	if ((j>=18)&&(j<width+HALF_WIND_SIZE)){
				mask = mask_in.read();
				img_pixel = img_in.read();
				blur_pixel = img_blur.read();
        	}
        	else{
        		mask = 0;
        		img_pixel = 0;
        		blur_pixel = 0;
        	}

            img_line.shift_pixels_up(j);
            img_line.insert_bottom_row(img_pixel, j);
            blur_line.shift_pixels_up(j);
            blur_line.insert_bottom_row(blur_pixel, j);
            mask_line.shift_pixels_up(j);
            mask_line.insert_bottom_row(mask, j);

            img_window.shift_pixels_left();
            blur_window.shift_pixels_left();
            mask_window.shift_pixels_left();

            for (int k = 0; k < WIND_SIZE; k++) {
#pragma HLS UNROLL factor=37
#pragma HLS LOOP_TRIPCOUNT max=37 min=37
                img_window.insert_pixel(img_line(k, j), k, WIND_SIZE-1);
                blur_window.insert_pixel(blur_line(k, j), k, WIND_SIZE-1);
                mask_window.insert_pixel(mask_line(k, j), k, WIND_SIZE-1);
            }

            if (i >= WIND_SIZE-1 && j >= WIND_SIZE-1){
					if (mask_window(18,36)){
						data_write.flag_begin=1;
					data_write.x = j - 18;
						data_write.y = i - HALF_WIND_SIZE;
						N = 0;
					}else{
						data_write.flag_begin=0;
					}
					if (mask_window(18,0)){
						data_write.flag_end=1;
					}else{
						data_write.flag_end=0;
					}
					if(N < WIND_SIZE){
						data_write.flag_last = 0;
						for (int i=0;i<37;i++){
							data_write.col_img.range((i*8+7),i*8) = img_window(i,HALF_WIND_SIZE);
							data_write.col_blur.range((i*8+7),i*8) = blur_window(i,HALF_WIND_SIZE);
						}
						N = N + 1;
						data_write.response =  mask_window(36,0);
						window_col.write(data_write);
					}

            }
        }
    }
    data_write.flag_last = 1;
    data_write.flag_begin=0;
    data_write.flag_end=0;
    window_col.write(data_write);
}

template<int W, int TYPE, int HEIGHT, int WIDTH, int NPC>
void process_rBRIEF_des_compute(
    hls::stream<WindowCol>& src,
    hls::stream<ap_axiu<16*2+8+256, 1, 1, 1>>& dst,
    int& length
) {

    ap_axiu<16*2+8+256, 1, 1, 1> dst_axi;
    WindowCol data;
    ap_uint<1> flag_last = 0;
    ap_uint<1> flag_stop_shift = 0;
    ap_fixed<ANGLE_WIDTH, 3> angle;

    xf::cv::Window<WIND_SIZE, WIND_SIZE, ap_uint<W>> img_window, blur_window;

    int cnt = 0;
    while (flag_last == 0) {
        data = src.read();
        flag_last = data.flag_last;
        if (flag_last == 0) {
            img_window.shift_pixels_left();
            blur_window.shift_pixels_left();
            for (int k = 0; k < 37; k++) {
#pragma HLS UNROLL factor=37
#pragma HLS LOOP_TRIPCOUNT max=37 min=37
                img_window.insert_pixel(data.col_img.range((8*k+7),8*k), k, 36);
                blur_window.insert_pixel(data.col_blur.range((8*k+7),8*k), k, 36);
            }
            flag_stop_shift = data.flag_end;
            if(flag_stop_shift == 1){
                dst_axi.data.range(16*2+8+256-1, 16+8+256) = data.x;
                dst_axi.data.range(16+8+256-1, 8+256) = data.y;
                dst_axi.data.range(8+256-1, 256) = data.response;
                angle = IC_Angle_31(img_window);
                dst_axi.data.range(256-1, 0) = descriptor(blur_window, angle);
                dst_axi.last = 0;
                dst_axi.keep = -1;
                idxTree[data.response][cntTree[data.response]] = cnt;
                cntTree[data.response]++;
                cnt++;
                dst.write(dst_axi);
            }
        }
    }
    length = cnt;
}


template<int W, int TYPE, int HEIGHT, int WIDTH, int NPC>
void process_output(
    hls::stream<DescOut>& src,
    hls::stream<ap_axiu<16*2+8+256, 1, 1, 1>>& dst,
    int& length
) {

    ap_axiu<16*2+8+256, 1, 1, 1> dst_axi;
    DescOut data;
    ap_uint<1> flag = 1;
    int cnt = 0;
    while (flag == 1) {
        data = src.read();
        flag = data.valid;
        if (flag == 1) {
            dst_axi.data.range(16*2+8+256-1, 16+8+256) = data.x;
            dst_axi.data.range(16+8+256-1, 8+256) = data.y;
            dst_axi.data.range(8+256-1, 256) = data.response;
            dst_axi.data.range(256-1, 0) = data.descriptor;
            dst_axi.last = 0;
            dst_axi.keep = -1;
            idxTree[data.response][cntTree[data.response]] = cnt;
            cntTree[data.response]++;
            cnt++;
            dst.write(dst_axi);
        }
    }
    length = cnt;
}

template<int W, int TYPE, int HEIGHT, int WIDTH, int NPC>
void process_ORB(
    hls::stream<ap_axiu<W, 1, 1, 1>>& src,
    hls::stream<ap_axiu<16*2+8+256, 1, 1, 1>>& dst,
    int height, 
    int width,
    int height_new,
    int width_new,
    int threshold,
    int& length
) {
    xf::cv::Mat<TYPE, HEIGHT, WIDTH, NPC> img_mat(height, width);
    xf::cv::Mat<TYPE, HEIGHT, WIDTH, NPC> resized_mat(height_new, width_new);
    hls::stream<ap_uint<W>> img, resized_img, img1, img2, img3, mask2, mask3, blur1, blur2, blur3;
    hls::stream<DescOut> desc;
    hls::stream<WindowCol> windowcol;

    #pragma HLS stream variable=img depth=2
    #pragma HLS stream variable=resized_img depth=2
    #pragma HLS stream variable=img1 depth=4*WIDTH
    #pragma HLS stream variable=img2 depth=4*WIDTH
    #pragma HLS stream variable=img3 depth=4*WIDTH
    #pragma HLS stream variable=mask2 depth=2
    #pragma HLS stream variable=mask3 depth=2
    #pragma HLS stream variable=blur1 depth=2
    #pragma HLS stream variable=blur2 depth=4*WIDTH
    #pragma HLS stream variable=blur3 depth=4*WIDTH
    #pragma HLS stream variable=desc depth=2
    #pragma HLS stream variable=windowcol depth=200

    #pragma HLS DATAFLOW
    process_input<W, TYPE, HEIGHT, WIDTH, NPC>(src, img, height, width);
    process_stream2xfMat<W, TYPE, HEIGHT, WIDTH, NPC>(img, img_mat);
    process_resize<W, TYPE, HEIGHT, WIDTH, NPC>(img_mat, resized_mat);
    process_xfMat2stream<W, TYPE, HEIGHT, WIDTH, NPC>(resized_mat, resized_img);
    process_blur<W, TYPE, HEIGHT, WIDTH, NPC>(resized_img, img1, blur1, height_new, width_new);
    process_fast<W, HEIGHT, WIDTH, NPC>(img1, img2, blur1, blur2, mask2, height_new, width_new, threshold);
    process_nms<W, HEIGHT, WIDTH, NPC>(img2, img3, blur2, blur3, mask2, mask3, height_new, width_new);
    process_rBRIEF_col_sel<W, HEIGHT, WIDTH>(img3, blur3, mask3, windowcol, height_new, width_new);
    process_rBRIEF_des_compute<W, TYPE, HEIGHT, WIDTH, NPC>(windowcol, dst, length);
}

int ORB_accel(
    hls::stream<ap_axiu<8, 1, 1, 1>>& src,
    hls::stream<ap_axiu<16*2+8+256, 1, 1, 1>>& dst,
    int height,
    int width,
    int height_new,
    int width_new,
    int threshold
) {

    #pragma HLS interface axis port=src depth=80*60
    #pragma HLS interface axis port=dst depth=80*60
    #pragma HLS interface s_axilite port=height
    #pragma HLS interface s_axilite port=width
    #pragma HLS interface s_axilite port=height_new
    #pragma HLS interface s_axilite port=width_new
    #pragma HLS interface s_axilite port=threshold
    #pragma HLS interface s_axilite port=return

    int nFeatures;

    process_ORB<8, XF_8UC1, 1080, 1920, XF_NPPC1>(
        src,
        dst,
        height,
        width,
        height_new,
        width_new,
        threshold,
        nFeatures
    );

    ap_axiu<16*2+8+256, 1, 1, 1> dst_axi;
    int cnt = 0;
    for (int val = 0; val < 256; val++) {
        for (int i = 0; i < cntTree[255-val]; i++) {
#pragma HLS LOOP_TRIPCOUNT max=100 min=0
            if (cnt < nFeatures) {
                dst_axi.data = idxTree[255-val][i];
                dst_axi.keep = -1;
                if (cnt == nFeatures - 1) {
                    dst_axi.last = 1;
                } else {
                    dst_axi.last = 0;
                }
                dst.write(dst_axi);
                cnt++;
            }
        }
        cntTree[255-val] = 0;
    }

    return nFeatures;
}
