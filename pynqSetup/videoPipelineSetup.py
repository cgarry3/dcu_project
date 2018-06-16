import pynq
import PIL.Image
from pynq import MMIO
from pynq.lib.video import *

## load overlay
ol = pynq.Overlay("/home/xilinx/jupyter_notebooks/overlays/base/base.bit")

## video format setup
rgba = PixelFormat(24, COLOR_IN_RGB, COLOR_OUT_RGB)
mode = VideoMode(1280, 720, 24)

## setup hdmi ports
ol.video.hdmi_in.configure(rgba)
ol.video.hdmi_out.configure(mode, rgba)

ol.video.hdmi_in.start()
ol.video.hdmi_out.start()

ol.video.hdmi_in.tie(ol.video.hdmi_out)

### display input video frame
frame = ol.video.hdmi_in.readframe()
image = PIL.Image.fromarray(frame)
image

## display hierachry
ol?


## read register
# an IP is located at 0x40000000
myip = MMIO(0x43C80010,0x10000)

# Read from the IP at offset 0
myip.read(0)
