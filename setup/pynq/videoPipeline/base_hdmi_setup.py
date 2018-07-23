from pynq.overlays.base import BaseOverlay
from pynq.lib.video import *
import time

## Load overlay
base = BaseOverlay("base.bit")

## setup hdmi ports
hdmi_in = base.video.hdmi_in
hdmi_out = base.video.hdmi_out
hdmi_in.configure()
hdmi_out.configure(hdmi_in.mode)

## Start HDMI
hdmi_in.start()
hdmi_out.start()

## Tie HMDI input port to output port
hdmi_in.tie(hdmi_out)

## Wait
time.sleep(120)

## Close both Ports
hdmi_out.close()
hdmi_in.close()
