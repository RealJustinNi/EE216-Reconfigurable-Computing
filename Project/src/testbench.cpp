#include <iostream>
#include <iomanip>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "hls_stream.h"
#include "ORB.h"
using namespace std;

#define TYPE XF_8UC1
#define HEIGHT 1080
#define WIDTH 1920
#define NPC XF_NPPC1
#define DATA_WIDTH 8
#define FAST_TH 20

void test() {
    int width, height, channels;
	unsigned char *img = stbi_load("test.png", &width, &height, &channels, 0);
    int height_new = height;
    int width_new = width;
    cout << "input shape: (" << height << ", " << width << ")" << endl;

    hls::stream<ap_axiu<8, 1, 1, 1>> src;
    hls::stream<ap_axiu<16*2+8+256, 1, 1, 1>> dst;
    ap_axiu<8, 1, 1, 1> src_axi;
    ap_axiu<16*2+8+256, 1, 1, 1> dst_axi;

    int cnt = 0;

    int read_idx = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            src_axi.data = img[read_idx];
            src_axi.keep = -1;
            if (i == height-1 && j == width-1) {
                src_axi.last = 1;
            } else {
                src_axi.last = 0;
            }
            src.write(src_axi);
            read_idx++;
        }
    }

    int length = ORB_accel(src, dst, height, width, height_new, width_new, FAST_TH);
    cout << dec << "N features: " << length << endl;

    cnt = 0;
    ap_uint<256> desc;
    unsigned int data[8];
    int x, y, response;
    ap_uint<1> flag = 0;
    for (int i = 0; i < length; i++) {
        dst.read(dst_axi);
        flag = dst_axi.last;
        desc = dst_axi.data.range(255, 0);
        x = dst_axi.data.range(16*2+8+256-1, 16+8+256);
        y = dst_axi.data.range(16+8+256-1, 8+256);
        response = dst_axi.data.range(8+256-1, 256);
        for (int n = 0; n < 8; n++) {
            data[n] = desc.range(32*(n+1)-1, 32*n);
        }
        cout << dec << x << " " << y;
        cout << " " << dec << response;
        for (int n = 7; n >= 0; n--) {
            cout << " " << hex << uppercase << setw(8) << setfill('0') << data[n];
        }
        cout << endl;
    }

    flag = 0;
    while (flag == 0) {
        dst.read(dst_axi);
        flag = dst_axi.last;
    }
}

int main() {
    cout << "############################################" << endl;
    cout << "#             Begin Testbench              #" << endl;
    cout << "############################################" << endl;
    cout << endl;

    test();

    cout << endl;
    cout << "############################################" << endl;
    cout << "#              End Testbench               #" << endl;
    cout << "############################################" << endl;
    return 0;
}
