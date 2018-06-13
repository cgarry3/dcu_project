import pynq
from pynq.lib.video import *
ol = pynq.Overlay("/home/xilinx/jupyter_notebooks/overlays/base/base.bit")

rgba = PixelFormat(24, COLOR_IN_RGB, COLOR_OUT_RGB)
mode = VideoMode(1280, 720, 24)

ol.video.hdmi_in.configure(rgba)
ol.video.hdmi_out.configure(mode, rgba)

ol.video.hdmi_in.start()
ol.video.hdmi_out.start()

ol.video.hdmi_in.tie(ol.video.hdmi_out)
