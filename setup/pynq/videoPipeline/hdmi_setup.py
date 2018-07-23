import pynq
import cv2
import PIL.Image
import time
from pynq.lib.video import *


## Load overlay
ol = pynq.Overlay("/home/xilinx/jupyter_notebooks/overlays/base/edge/edge.bit")


## setup hdmi ports
ol.video.hdmi_in.configure()
ol.video.hdmi_out.configure(ol.video.hdmi_in.mode)

## Start HDMI
ol.video.hdmi_in.start()
ol.video.hdmi_out.start()

## Tie HMDI input port to output port
ol.video.hdmi_in.tie(ol.video.hdmi_out)

## Wait
time.sleep(120)

## Close both Ports
ol.video.hdmi_out.close()
ol.video.hdmi_in.close()
