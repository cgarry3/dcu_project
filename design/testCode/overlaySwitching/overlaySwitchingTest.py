import pynq
import cv2
import PIL.Image
import time
from pynq.lib.video import *

## start time 
start = time.time()


#####################################
#  First Overlay
#####################################

## load overlay
ol = pynq.Overlay("/home/xilinx/jupyter_notebooks/overlays/1080p/hdmi_only/gray/gray.bit")


## video format setup
ol.video.hdmi_in.configure()
ol.video.hdmi_out.configure(ol.video.hdmi_in.mode)


ol.video.hdmi_in.start()
ol.video.hdmi_out.start()


ol.video.hdmi_in.tie(ol.video.hdmi_out)

## wait 30 seconds
time.sleep(30)

ol.video.hdmi_out.close()
ol.video.hdmi_in.close()

#####################################
#  second Overlay
#####################################

## load overlay
ol = pynq.Overlay("/home/xilinx/jupyter_notebooks/overlays/1080p/hdmi_only/edge/edge.bit")


## video format setup
ol.video.hdmi_in.configure()
ol.video.hdmi_out.configure(ol.video.hdmi_in.mode)


ol.video.hdmi_in.start()
ol.video.hdmi_out.start()


ol.video.hdmi_in.tie(ol.video.hdmi_out)

## wait 30 seconds
time.sleep(30)

ol.video.hdmi_out.close()
ol.video.hdmi_in.close()

#####################################
#  Measure time
#####################################

end = time.time()
print(end-start-60)
