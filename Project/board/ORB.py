from pynq import Overlay, allocate
import numpy as np
import cv2
import time
import sys

def run_kernel(dma, kernel, inbuf, outbuf, H, W, H_new, W_new, th):
    dma.sendchannel.transfer(inbuf)
    dma.recvchannel.transfer(outbuf)
    kernel.write(0x18, H)
    kernel.write(0x20, W)
    kernel.write(0x28, H_new)
    kernel.write(0x30, W_new)
    kernel.write(0x38, th)
    kernel.write(0x0, 0x1)
    dma.sendchannel.wait()
    dma.recvchannel.wait()
    return kernel.read(0x10)

if __name__=='__main__':
    bitstream_file = sys.argv[1]

    print("*** Program FPGA with %s ***"%bitstream_file)
    ol = Overlay(bitstream_file)
    print("*** Program FPGA done ***")

    dma = ol.axi_dma_0
    kernel = ol.ORB_accel_0

    img1_file = "tum_fr1_xyz_1.png"
    img2_file = "tum_fr1_xyz_2.png"
    src1 = cv2.imread(img1_file, 0)
    src2 = cv2.imread(img2_file, 0)
    (height, width) = src1.shape
    height_new = height
    width_new = width

    max_output_size = height*width
    in_buffer = allocate(shape=(height, width), dtype=np.uint8)
    out_buffer = allocate(shape=(max_output_size, 37), dtype=np.uint8)

    print("input shape:", height, width)
    print("resized shape:", height_new, width_new)

    threshold = 60
    print("*** Run Single test ***")
    print("FAST threshold: %d"%threshold)
    in_buffer[:] = src1[:]
    N_keypoints = run_kernel(dma, kernel, in_buffer, out_buffer, height, width, height_new, width_new, threshold)
    print("Keypoints in img1: %d"%N_keypoints)
    kps1 = []
    desc1 = np.zeros(shape=(N_keypoints, 32), dtype=np.uint8)
    for i in range(N_keypoints):
        x = float((out_buffer[i, 36] << 8) | out_buffer[i, 35])
        y = float((out_buffer[i, 34] << 8) | out_buffer[i, 33])
        response = out_buffer[i, 32]
        kp = cv2.KeyPoint()
        kp.pt = (x, y)
        kps1.append(kp)
        for k in range(32):
            desc1[i, k] = out_buffer[i, 31 - k]

    in_buffer[:] = src2[:]
    N_keypoints = run_kernel(dma, kernel, in_buffer, out_buffer, height, width, height_new, width_new, threshold)
    print("Keypoints in img2: %d"%N_keypoints)
    kps2 = []
    desc2 = np.zeros(shape=(N_keypoints, 32), dtype=np.uint8)
    for i in range(N_keypoints):
        x = float((out_buffer[i, 36] << 8) | out_buffer[i, 35])
        y = float((out_buffer[i, 34] << 8) | out_buffer[i, 33])
        response = out_buffer[i, 32]
        kp = cv2.KeyPoint()
        kp.pt = (x, y)
        kps2.append(kp)
        for k in range(32):
            desc2[i, k] = out_buffer[i, 31 - k]

    print("*** Match and draw keypoints ***")
    bf = cv2.BFMatcher_create(cv2.NORM_HAMMING, crossCheck=False)
    matches = bf.knnMatch(desc1, desc2, k=2)
    good_matches = []
    for m, n in matches:
        if m.distance < 0.5 * n.distance:
            good_matches.append(m)
    img_out = cv2.drawMatches(src1, kps1, src2, kps2, good_matches, None, flags=2)
    cv2.imwrite("matches.png", img_out)

    print("*** Run Multiple test ***")
    threshold = 40
    N_runs = 20
    in_buffer[:] = src1[:]
    
    t_start = time.time()
    for i in range(N_runs):
        run_kernel(dma, kernel, in_buffer, out_buffer, height, width, height_new, width_new, threshold)
    t_end = time.time()
    t = (t_end-t_start)*1000/N_runs

    print("FAST threshold: %d"%threshold)
    print("Average latency: %.8f ms"%t)
