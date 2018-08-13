import pynq
import cv2
import PIL.Image
import time
from pynq.lib.video import *

## start time 
start = time.time()




## load overlay
ol = pynq.Overlay("/home/xilinx/jupyter_notebooks/overlays/1080p/motorway/motorway.bit")

#####################################
#  Register Memory setup
#####################################

IP_BASE_ADDRESS = 0x43C70000
ADDRESS_RANGE = 0x10000
ADDRESS_OFFSET = 0x10

from pynq import MMIO
mmio = MMIO(IP_BASE_ADDRESS, ADDRESS_RANGE)

#####################################
#  Register Read
#####################################
x=0
while(x<5):
    result = mmio.read(ADDRESS_OFFSET)
    print("Result is: " + str(result))
    end = time.time()
    print(end-start)
    x=x+1
