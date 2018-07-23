#############################################################
#  Name:          MQTT Publisher
#  Aurthor:       Cathal Garry
#  Description:   The MQTT publisher reads the number
#                 cars that have passed since the last
#                 message and publishes it to the broker
#############################################################

from pynq import MMIO
import paho.mqtt.client as mqtt
import random
import time
import pynq
import cv2
import PIL.Image
from pynq.lib.video import *
from collections import deque

######################################
## Video Processing Variables
######################################

## Details of the image
numOfLanes = 6
numOfLeftLanes = numOfLanes/2
numOfRightLanes = numOfLanes/2
frameRatePerSecond = 25
timePerFrame = 1/frameRatePerSecond

######################################
## MQTT Variables
######################################

## General Setup
QoS      = 1
user     = "garryc3"
password = "password"
port     = 1883
brokerIP = "192.168.0.39"

## MQTT Last Will
lwm='unexcepted exit' # Last will message
lwTopic='ee580/m3P0'

## Rate of messages published per second
publishRate = 1
timeBetweenPublishs = publishRate/timePerFrame

######################################
## Custom IP Register Variables
######################################

IP_BASE_ADDRESS = 0x43C70000
ADDRESS_RANGE = 0x10000
RESULT_REG_OFFSET = 0x10


## Lane selection initialization
laneSelect = []
for i in range(numOfLanes):
    singleLaneSelect = 1 << i;
    laneSelect.append(singleLaneSelect)
    
## Vehicle counts
presentVehicleCount = []
previousVehicleCount = []
for i in range(numOfLanes):
    presentVehicleCount.append(0)
    previousVehicleCount.append(0)
    
## Used to determine if a new vehicle has passed
numOfFramesChecked = 8
allLanesResults = []
for i in range(numOfLanes):
    LaneResults = [0] * numOfFramesChecked
    allLanesResults.append(LaneResults)

########################################################################################
## Functions 
########################################################################################

######################################
## Video Processing Functions 
######################################

def setupHDMIPipe():
    ## load overlay for vehicle counting IP
    ol = pynq.Overlay("/home/xilinx/jupyter_notebooks/overlays/debug/debug8/debug8.bit")

    ## Configuring input and output video streams
    ol.video.hdmi_in.configure()
    ol.video.hdmi_out.configure(ol.video.hdmi_in.mode)

    ## Start HDMI Stream In&Out
    ol.video.hdmi_in.start()
    ol.video.hdmi_out.start()

    ## Connecting HDMI directly to HDMI out
    ol.video.hdmi_in.tie(ol.video.hdmi_out)
    
def closeHDMIPipe():
    ## Closing HDMI stream In&Out
    ol.video.hdmi_out.close()
    ol.video.hdmi_in.close()

def readResultReg():
    result = mmio.read(RESULT_REG_OFFSET)
    return result;


###############################################
##   MQTT Functions
###############################################

def on_connect(mosq, obj, rc):
    mqttc.subscribe("f", 0)
    print("rc: " + str(rc))

def on_message(mosq, obj, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))
    message = msg.payload
    mqttc.publish("f2",msg.payload);

def on_publish(mosq, obj, mid):
    print("mid: " + str(mid))

def on_subscribe(mosq, obj, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

def on_log(mosq, obj, level, string):
    print(string)
 
def mqttSetup(ipAddress, portNum, username, pwd):
    ## Assign event callbacks
    mqttc.on_message = on_message
    mqttc.on_connect = on_connect
    mqttc.on_publish = on_publish
    mqttc.on_subscribe = on_subscribe

    ## set user name and password
    mqttc.username_pw_set(username, password=pwd)

    ## last will setup
    mqttc.will_set(lwTopic, lwm, QoS, retain=False)

    ## Connect
    mqttc.connect(ipAddress, portNum,60)

########################################################################################
## Execution
########################################################################################

#####################################
## MQTT Publisher Setup
#####################################

## create client objec
mqttc = mqtt.Client()

## setup MQTT publisher
mqttSetup(brokerIP, port, user, password)

#####################################
## Setup Video Pipeline
#####################################

setupHDMIPipe()

#####################################
## Register Read Setup
#####################################

mmio       = MMIO(IP_BASE_ADDRESS, ADDRESS_RANGE)
frameCount = 0

## processing result from Custom IP
while True:
    result = readResultReg()
    
    ###########################
    ## Count Vehicles
    ###########################
    
    for x in range(numOfLanes):
        resultTemp = (laneSelect[x] & result) >> x
        ## remove oldest result in the list
        allLanesResults[x].pop(0)
        ## add new result
        allLanesResults[x].append(resultTemp)
        
        ## check if a new vehicle has crossed the count line
        newVehicleCrossLine = False
        for y in range(len(allLanesResults[x])-1, 0, -1): 
            if( y==len(allLanesResults[x])-1 and allLanesResults[x][y]==1):
                newVehicleCrossLine=True
            elif( allLanesResults[x][y]==1):
                newVehicleCrossLine=False
        
        ## Increase count
        if(newVehicleCrossLine==True):
            presentVehicleCount[x]=presentVehicleCount[x]+1
            print("Lane" + str(x) + " count: " + str(presentVehicleCount[x]))
            
        print("all results" + str(x) + ":")
        print(allLanesResults[x])
    print("all vechicle counts:")
    print(presentVehicleCount)
        
    
    ###########################
    ## Publish Results
    ###########################
    
    if(frameCount==timeBetweenPublishs):
        topic   =  'ee580/m3P0'
        message = '{d:{Number of Vehicles:' + str(random.randint(0, 5)) + '}}'
        mqttc.publish(topic,message)
        frameCount = 0

    ###########################
    ## Sleep Between Frames
    ###########################
    
    frameCount = frameCount + 1
    time.sleep(timePerFrame)
    
    
