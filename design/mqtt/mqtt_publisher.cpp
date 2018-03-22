// Based on the Paho C code example from www.eclipse.org/paho/
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "MQTTClient.h"
#include "MQTTClient.h"

using namespace std;

//Please replace the following address with the address of your server
#define ADDRESS         "tcp://192.168.0.29:1883"
#define CLIENTID        "rpi1"
#define AUTHMETHOD      "garryc3"
#define AUTHTOKEN       "password"
#define TOPIC_TEMP      "ee513/temp"
#define QOS             2
#define TIMEOUT         10000L
#define LASTWILLTOPIC   "/ee513/temp"
#define LASTWILLQOS     2
#define LASTWILLMESS    "unexcepted exit"
#define LASTWILLRETAIN  false
#define NUM_PUBLISHS    500



int main(int argc, char* argv[]) {
    

   // ---------------------------------------------
   // creating last will
   // ---------------------------------------------
   
   MQTTClient_willOptions will_opts = MQTTClient_willOptions_initializer;
   will_opts.topicName   = LASTWILLTOPIC;
   will_opts.qos         = LASTWILLQOS;
   will_opts.message     = LASTWILLMESS;
   will_opts.retained    = LASTWILLRETAIN;
   
   // ---------------------------------------------
   // Creating MQTT Publisher
   // --------------------------------------------- 
   
   MQTTClient client;
   MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
   MQTTClient_message pubmsg = MQTTClient_message_initializer;
   MQTTClient_deliveryToken token;
   MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
   opts.will = &will_opts;
   opts.keepAliveInterval = 20;
   opts.cleansession = 1;
   opts.username = AUTHMETHOD;
   opts.password = AUTHTOKEN;
   

   
   int rc;
   if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS) {
      cout << "Failed to connect, return code " << rc << endl;
      return -1;
   }
   
   // ---------------------------------------------
   // Publishing Messages
   // ---------------------------------------------
   
   for(int x=0; x<NUM_PUBLISHS; x=x+1) {           
           // ------------------------------------------
           // Sending the Temperature + 2sec sleep
           // ------------------------------------------
           
           stringstream message_str;
           message_str << "{\"d\":{\"Number of Vehicles\":" << "12" << "}}" << '\n';
           pubmsg.payload = (char*) message_str.str().c_str();
           pubmsg.payloadlen = message_str.str().length();
           pubmsg.qos = QOS;
           pubmsg.retained = 0;
           MQTTClient_publishMessage(client, TOPIC_TEMP, &pubmsg, &token);
           cout << "Waiting for up to " << (int)(TIMEOUT/1000) <<
                " seconds for publication of " << "hello from the zybo board" <<
                " \non topic " << TOPIC_TEMP << " for ClientID: " << CLIENTID << endl;
           rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
           cout << "Message with token " << (int)token << " delivered." << endl;
           usleep(200000);

           
           // Sleep for 40 seconds
           usleep(4000000);
   }
   
   // ---------------------------------------------
   // Closing MQTT Publisher
   // ---------------------------------------------
   
   MQTTClient_disconnect(client, 10000);
   MQTTClient_destroy(&client);
   return rc;
}
