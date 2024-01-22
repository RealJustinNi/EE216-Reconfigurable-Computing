from pynq import Overlay, allocate
import numpy as np
import time
import cv2

WIDTH = 1280
HEIGHT = 720
WIDTHOUT = 1278
HEIGHTOUT = 718

# Download bitstream
print("programing FPGA...")
overlay = Overlay("/root/junjun/sobel.bit")
print("programing FPGA done")

# Get the IP object from the overlay
sobel = overlay.sobel_0
dma = overlay.axi_dma_0

# Get input image and golden result
img_in = cv2.imread("/root/junjun/input.png", cv2.IMREAD_GRAYSCALE) 
out_ref = cv2.imread("/root/junjun/reference.png", cv2.IMREAD_GRAYSCALE)

# Allocate buffers
in_buffer = allocate(shape=(WIDTH*HEIGHT,), dtype=np.uint8)
out_buffer = allocate(shape=(WIDTHOUT*HEIGHTOUT,), dtype=np.uint8)

# Initialize input data
for i in range(HEIGHT):
    for j in range(WIDTH):
        in_buffer[i*WIDTH+j] = img_in[i][j]

print('Testing sobel performance for 1000 rounds...')
N = 1000
start_time = time.time() 
for t in range(N):
    # Launch the IP and DMA
    sobel.write(0x18,HEIGHT)
    sobel.write(0x20,WIDTH)
    sobel.write(0x0, 0x1)
    dma.sendchannel.transfer(in_buffer)
    dma.recvchannel.transfer(out_buffer)
    dma.sendchannel.wait()
    dma.recvchannel.wait()
end_time = time.time()
fps = N / (end_time - start_time)
print("fps =", fps)

# Allocate result and evalution
print('Evaluting result...')
result = allocate(shape=(HEIGHTOUT,WIDTHOUT),dtype=np.uint8)
error = 0
for i in range(HEIGHTOUT):
    for j in range(WIDTHOUT):
        result[i][j] = out_buffer[i*WIDTHOUT+j]
        if result[i][j] != out_ref[i][j]:
            error += 1
print(f'The output has {error} errors.')
cv2.imwrite("/root/junjun/result.png", result)
print('The result figure is saved.')