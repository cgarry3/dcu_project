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
## Debug Mode
##  - Set to one to print debug msg
######################################

DEBUG = 0

######################################
## Video Processing Variables
######################################

## Details of the image
numOfLanes = 6
numOfLeftLanes = numOfLanes/2
numOfRightLanes = numOfLanes/2
frameRatePerSecond = 25
timePerFrame = 1/frameRatePerSecond

## Path to Overlay
overlayPath = "/home/xilinx/jupyter_notebooks/overlays/1080p/hdmi_only/motorway/motorway.bit"

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
RESULT_REG_OFFSET = 0x30
LEFT_COUNT_REG_OFFSET = 0x18
RIGHT_COUNT_REG_OFFSET = 0x28

######################################
## Congestion Variables
######################################

## Lane selection initialization
laneSelect = []
for i in range(numOfLanes):
    singleLaneSelect = 1 << i;
    laneSelect.append(singleLaneSelect)
    
## Congestion Delays
congestionDelays = []
for i in range(numOfLanes):
    congestionDelays.append(0)
    
## Vehicle counts
VehicleCount = []
for i in range(numOfLanes):
    VehicleCount.append(0)
    
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
    ## Configuring input and output video streams
    ol.video.hdmi_in.configure()
    ol.video.hdmi_out.configure(ol.video.hdmi_in.mode)

    ## Start HDMI Stream In&Out
    ol.video.hdmi_in.start()
    ol.video.hdmi_out.start()

    ## Connecting HDMI directly to HDMI out
    ol.video.hdmi_in.tie(ol.video.hdmi_out)
    time.sleep(30)
    
def closeHDMIPipe():
    ## Closing HDMI stream In&Out
    ol.video.hdmi_out.close()
    ol.video.hdmi_in.close()

def readLeftCountReg():
    result = mmio.read(LEFT_COUNT_REG_OFFSET)
    return result;
    
def readRightCountReg():
    result = mmio.read(RIGHT_COUNT_REG_OFFSET)
    return result;
 
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
print("############### Setting up MQTTT Publisher ###############")
mqttc = mqtt.Client()

## setup MQTT publisher
mqttSetup(brokerIP, port, user, password)


#####################################
## Setup Video Pipeline
#####################################

## load overlay for vehicle counting IP
ol = pynq.Overlay(overlayPath)

## Setup HDMI Pipeline
print("############### Setting up HDMI Pipeline ###############")
setupHDMIPipe()


#####################################
## Register Read Setup
#####################################

print("############### Setting up Register Access ###############")
mmio       = MMIO(IP_BASE_ADDRESS, ADDRESS_RANGE)
frameCount = 0

####################################################
## Determining congestion based off result value
####################################################

while True:
    ## Result readback value
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

        ##################################
        # Count
        ##################################
        
        ## check if a new vehicle has crossed the count line
        newVehicleCrossLine = False
        for y in range(len(allLanesResults[x])-1, 0, -1): 
            if( y==len(allLanesResults[x])-1 and allLanesResults[x][y]==1):
                newVehicleCrossLine=True
            elif( allLanesResults[x][y]==1):
                newVehicleCrossLine=False
        
        ## Increase count
        if(newVehicleCrossLine==True):
            VehicleCount[x]=VehicleCount[x]+1
            if(DEBUG==1):
                print("Lane" + str(x) + " count: " + str(VehicleCount[x]))
       
       
        ##################################
        ## Increase congestion count
        ##  - congestion count increase will there is a car being tracked
        ##  - when all the list==0 the car has passed
        ##################################
        
        if(sum(allLanesResults[x])==0 and congestionDelays[x]!=0):
            if(DEBUG==1):
                print("Lane" + str(x) + " Max Delay: " + str(congestionDelays[x]))
            congestionDelays[x]=0
        else:
            congestionDelays[x]=congestionDelays[x]+1
        
        if(DEBUG==1):    
            print("all results" + str(x) + ":")
            print(allLanesResults[x])
            print(congestionDelays[x])
    if(DEBUG==1):
        print("Current count")
        print(VehicleCount)
    
    ###########################
    ## Publish Results
    ###########################
    
    if(frameCount==timeBetweenPublishs):
        ## print out 
        print("############### Publishing Data ###############")

        ## Read Count values
        leftLaneCount =sum(VehicleCount[0:2])
        rightLaneCount =sum(VehicleCount[3:5])

        ## Publish left lane count
        topic   =  'ee580/m3p0_left_count'
        message = '{d:{Left Lane Count:' + str(leftLaneCount) + '}}'
        mqttc.publish(topic,message)
        
        ## Publish right lane count
        topic   =  'ee580/m3p0_right_count'
        message = '{d:{Right Lane Count:' + str(rightLaneCount) + '}}'
        mqttc.publish(topic,message)
        
        ## Publish max congestion(mseconds)
        maxCongestion = max(congestionDelays)
        topic   =  'ee580/m3p0_congestion'
        message = '{d:{Max Congestion:' + str(int(maxCongestion*timePerFrame)) + '}}'
        mqttc.publish(topic,message)
        
        ## Reset frame count
        frameCount = 0

    ###########################
    ## Sleep Between Frames
    ###########################
    
    frameCount = frameCount + 1
    time.sleep(timePerFrame)
    
closeHDMIPipe()
    
    
