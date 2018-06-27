#############################################################
#  Name:          MQTT Publisher
#  Aurthor:       Cathal Garry
#  Description:   The MQTT publisher reads the number
#                 cars that have passed since the last
#                 message and publishes it to the broker
#############################################################

import paho.mqtt.client as mqtt
import time

## rate of messages published per second
publishRate = 1

def setupHDMIPipe():
    ## add code here for setting up HDMI pipe
    
def readResultReg():
    ## add code here to read result from 
    ## custom IP in HDMI pipe

def on_connect(mosq, obj, rc):
    mqttc.subscribe("f", 0)
    print("rc: " + str(rc))

def on_message(mosq, obj, msg):
    message = '{d:{Number of Vehicles:' + randint(0, 5) + '}}'
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))
    message = msg.payload
    mqttc.publish("f2",msg.payload);

def on_publish(mosq, obj, mid):
    print("mid: " + str(mid))

def on_subscribe(mosq, obj, mid, granted_qos):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

def on_log(mosq, obj, level, string):
    print(string)

mqttc = mqtt.Client()

while True:
    # Assign event callbacks
    mqttc.on_message = on_message
    mqttc.on_connect = on_connect
    mqttc.on_publish = on_publish
    mqttc.on_subscribe = on_subscribe
    
    # Connect
    mqttc.connect("localhost", 1883,60)
    
    # sleep
    time.sleep(publishRate)


# Continue the network loop
#mqttc.loop_forever()
