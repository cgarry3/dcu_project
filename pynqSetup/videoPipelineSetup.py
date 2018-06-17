import pynq
import cv2
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

########################################
#  Image processing
########################################

## draw rectangle
def drawRec(x0, y0, x1, y1, img):
        cv2.rectangle(img,(x0,y0),(x1,y1),(255,0,0),3)
        
## draw count
def drawCount(x0, y0, count, img):
        font = cv2.FONT_HERSHEY_SIMPLEX
        cv2.putText(img, str(count), (x0, y0), font, 4, (255,0,0), 2, cv2.LINE_AA)

########################################
#  car counting
########################################

leftLaneCount       = 0
rightLaneCount      = 0
numberOfLanes       = 6
numframes           = 300
selectResults       = [1, 2, 4, 8, 16, 32]
VehicleCount        = [0, 0, 0, 0, 0, 0]
framesSinceLastSeen = [0, 0, 0, 0, 0, 0]
framesOfTheSameCar  = [0, 0, 0, 0, 0, 0]

###### Frame Processing ###################



## start time 
start = time.time()


## loop through 30 frames
for y in range(numframes):
    ## read result register
    #result = myip.read(0)
    result = 1

    ## input and output frame    
    inframe = ol.video.hdmi_in.readframe()
    outframe = ol.video.hdmi_out.newframe()
    
    ## loop through each lane
    for x in range(numberOfLanes):
            ## frame does have car in ROI
            if (result and selectResults[x]) != 0 :
                if framesSinceLastSeen[x]>5 :
                    # reset to zero
                    framesSinceLastSeen[x] = 0
                    framesOfTheSameCar[x] = 0
                    ## increment vehicle count
                    VehicleCount = VehicleCount + 1
                elif framesSinceLastSeen[x]>5 : 
                    # count the number of frames of the same car
                    framesOfTheSameCar[x] = framesOfTheSameCar[x] + 1
                    
            ## frame doesn't have car in ROI
            else:
                framesSinceLastSeen[x] = framesSinceLastSeen[x] + 1
                
    ## update count on image
    drawCount(0, 100, leftLaneCount, inframe)
    drawCount(0, 700, rightLaneCount, inframe)
    
    ## draw retangles
    drawRec(0, 100, 40, 200, inframe) 

    ## output new frame
    #outframe =  inframe
    inframe.freebuffer()
    cv2.Laplacian(inframe, cv2.CV_8U, dst=outframe)
    inframe.freebuffer()
    image = PIL.Image.fromarray(outframe)
    image
    ol.video.hdmi_out.writeframe(outframe)


    
end = time.time()
print("Frames per second:  " + str(numframes / (end - start)))

            
###### End Frame Processing ###################  


### Close HDMI #######
ol.video.hdmi_out.close()
ol.video.hdmi_in.close()      
