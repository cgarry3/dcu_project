#############################################################
#  Name:          MQTT Publisher
#  Aurthor:       Cathal Garry
#  Description:   The MQTT publisher reads the number
#                 cars that have passed since the last
#                 message and publishes it to the broker
#############################################################

import paho.mqtt.client as mqtt
import time

## MQTT setup
user     = "garryc3"
password = "password"
port     = 1883

## MQTT last will
lwm="-1" # Last will message
lwTopic="ee580/m3P0"

## Rate of messages published per second
publishRate = 1

def setupHDMIPipe(mosq, obj):
    ## add code here for setting up HDMI pipe
    
def readResultReg(mosq, obj):
    ## add code here to read result from 
    ## custom IP in HDMI pipe

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

## create client objec
mqttc = mqtt.Client()

## Assign event callbacks
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_publish = on_publish
mqttc.on_subscribe = on_subscribe

## set user name and password
client.username_pw_set(user, password=password)

## last will setup
client.will_set(lwTopic,lwm,QOS1,retain=False)

## Connect
mqttc.connect("localhost", port,60)

while True:
    ## publish data
    topic   =  'ee580/m3P0'
    message = '{d:{Number of Vehicles:' + randint(0, 5) + '}}'
    client1.publish(topic,message) 
    
    # sleep
    time.sleep(publishRate)


# Continue the network loop
#mqttc.loop_forever()
