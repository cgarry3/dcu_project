// Based on the Paho C code example from www.eclipse.org/paho/
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "MQTTClient.h"


using namespace std;

#define ADDRESS     "tcp://192.168.0.29:1883"
#define CLIENTID    "rpi"
#define AUTHMETHOD  "garryc3"
#define AUTHTOKEN   "password"
#define TOPIC       "ee580/#"
#define QOS         1
#define TIMEOUT     10000L

// Used for controlling LED
#define GPIO            "/sys/class/gpio/"
#define GPIO_NO         23
#define LWNUMLEDFLASHES 5

// default congestion leve
#define DEFAULT_NUM_OF_VEHICLES 3

// sample of congestion results
// four samples gives us an estimate over 2 minutes
#define SAMPLE_SIZE 4

volatile MQTTClient_deliveryToken deliveredtoken;

// ---------------------------------------------
// Number of Vehicle passed on motorway
// a sample is recieved every 1 second
// 0 = Accident has occured (50kph)
// 1 = Very slow traffic or possible accident (80kph)
// 2 = High Traffic  (80kph)
// 3 = Medium flow of traffic (100kph)
// 4 = fast flow of traffic (120kph)
// ---------------------------------------------

int numberOfVehiclesPassed[SAMPLE_SIZE];

// ---------------------------------------------
// Motorway speed(KMPH)
// - based on the average of the congestion 
//   level over two minutes
// ---------------------------------------------

int motorway_speed = 100; 

// ---------------------------------------------
// Update motorway speed
// ---------------------------------------------

int updateMotorWaySpeed()
{
    // initial average
    int average = 0;
    
    for(int x=0; x<SAMPLE_SIZE; x++)
    {
        average = numberOfVehiclesPassed[x] + average;
    }
    
    // final average

    average = (average/SAMPLE_SIZE);

    // new motorway speed based on the average
    switch(average) {
        case 0 : motorway_speed = 50;  // Accident has occur traffic is stalled
                 break;
        case 1 : motorway_speed = 80;  // High Traffic or possible accident
                 break;
        case 2 : motorway_speed = 80;  // High Traffic
                 break;
        case 3 : motorway_speed = 100; // Medium flow of traffic
                 break;
        case 4 : motorway_speed = 120; // fast flow of traffic
                 break;
        default: motorway_speed = 80;  // default
                 break;
    }

    
    cout << "New motorway speed: " << motorway_speed << endl;
    
    // returns new average
    return average;
}

// ---------------------------------------------
// Update number of cars
// ---------------------------------------------

void updateNumberOfVehiclesPassed(int newResult)
{
   for(int x=1; x<SAMPLE_SIZE; x++)
   {
       // shift everything to the left by one
       numberOfVehiclesPassed[x] = numberOfVehiclesPassed[x-1];
   }
   
   // load new value
   numberOfVehiclesPassed[0] = newResult;
}

// ---------------------------------------------
// Used to indicate if the string is a 
// float value or not, returns a boolean value
// ---------------------------------------------

bool isFloat( string myString ) {
    std::istringstream iss(myString);
    float f;
    iss >> noskipws >> f; // noskipws considers leading whitespace invalid
    // Check the entire string was consumed and if either failbit or badbit is set
    return iss.eof() && !iss.fail(); 
}

// ---------------------------------------------
// Used to indicate if the string is a 
// number or not, returns a boolean value
// ---------------------------------------------

bool is_number(string line)
{  
    // check if line is not blank
    if(!line.empty()){
        char* p;
        strtol(line.c_str(), &p, 10);
        return *p == 0;
    }
    
    return 0;
    
}

// ---------------------------------------------
// Used to split strings
// ---------------------------------------------

string split(string input_string, string delimiter, int sel_split) {
      string token;
      int split_no=0;
      int del;

      while (split_no < sel_split) {
                del = input_string.find(delimiter);
                token = input_string.substr(0, del);
                input_string.erase(0, del + delimiter.length());
                split_no += 1;
      }
      return input_string.substr(0, input_string.find(delimiter));
}


//---------------------------------------------
// Setup GPIO 23
// -------------------------------------------

void setupGPIO(){
        // Setup GPIO
        string gpioPath = string(GPIO "gpio") + to_string(GPIO_NO) + string("/");
    
        string export_str = "/sys/class/gpio/export";
        ofstream exportgpio(export_str.c_str());
        //if (exportgpio < 0){
        //cout << " OPERATION FAILED: Unable to export GPIO"<< GPIO_NO <<" ."<< endl;
        //}

        exportgpio << GPIO_NO ; //write GPIO number to export
        exportgpio.close(); //close export file
        
        // ----------------------------------------------------
        // Setting to output
        // ----------------------------------------------------
        
        export_str = gpioPath + "direction";
        ofstream gpio_direction(export_str.c_str());
        //if (gpio_direction < 0){
        //cout << " OPERATION FAILED: Unable to export GPIO"<< GPIO_NO <<" ."<< endl;
        //}

        gpio_direction << "out" ; //write GPIO number to export
        gpio_direction.close(); //close export file   
}
//---------------------------------------------
// Turn On LED using GPIO 23
// -------------------------------------------

void turnOnLed() {
        // Setup GPIO Path
        string gpioPath = string(GPIO "gpio") + to_string(GPIO_NO) + string("/");
    
    
        // ---------------------------------------------------
        // Setting up GPIO 23
        // ---------------------------------------------------
    
        setupGPIO();
        
        // ----------------------------------------------------
        // Setting to output Value
        // ----------------------------------------------------
        
        string value_str = gpioPath + "value";
        ofstream gpio_value(value_str.c_str());
        //if (gpio_value < 0){
        //cout << " OPERATION FAILED: Unable to export GPIO"<< GPIO_NO <<" ."<< endl;
        //}

        gpio_value << 1 ; //write GPIO number to export
        gpio_value.close(); //close export file
    
}

//----------------------------------------
// Turns Off LED using GPIO 23
//----------------------------------------

void turnOffLed() {
        // Setup GPIO Path
        string gpioPath = string(GPIO "gpio") + to_string(GPIO_NO) + string("/");
        
        // ---------------------------------------------------
        // Setting up GPIO 23
        // ---------------------------------------------------
    
        setupGPIO();
        
        // ----------------------------------------------------
        // Setting to output Value
        // ----------------------------------------------------
        
        string value_str = gpioPath + "value";
        ofstream gpio_value(value_str.c_str());
        //if (gpio_value < 0){
        //cout << " OPERATION FAILED: Unable to export GPIO"<< GPIO_NO <<" ."<< endl;
        //}

        gpio_value << 0 ; //write GPIO number to export
        gpio_value.close(); //close export file
}

void delivered(void *context, MQTTClient_deliveryToken dt) {
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    
    // -------------------------------------------------------
    // Variables
    // -------------------------------------------------------
    
    int      i;              // Used in for loop
    bool     last_will;      // Indicates if it is a last will packet
    char*    payloadptr;     // Used to contain payload
    bool     isNumber;       // Indicates if it is a number or not
    float    data;           // packet data
    string   topic;          // packet topic 
    
    // JSON parseing variables
    string                   mqtt_message;
    string                   mqtt_message2;
    string                   mqtt_message3;
    
    // -------------------------------------------------------
    // Getting Payload
    // -------------------------------------------------------
    
    payloadptr = (char*) message->payload;
    for(i=0; i<message->payloadlen; i++) {
        mqtt_message += *payloadptr++;
    }
    
    // -------------------------------------------------------
    // Checking to see if it is a last will message
    // -------------------------------------------------------
    
    if(mqtt_message == "unexcepted exit") {
        cout << mqtt_message << "\n";
        last_will = 1;
    }
    else {
        last_will = 0;
        // Striping data out of JSON packet
        mqtt_message2 = split(mqtt_message, ":", 2);
        mqtt_message3 = split(mqtt_message2, "}", 0);
    }  
    
    // -------------------------------------------------------
    // Checking to see if the data is corrupted
    // -------------------------------------------------------
    
    isNumber = is_number(mqtt_message3);
    
    if(isNumber==1) {
            data = stof(mqtt_message3);
            topic += split(topicName, "/",1);
            cout << "Topic: " << topic << "\nData: " << data << "\n";
    } else if(last_will==0) {
            cout << "Dropping Packet!\n";
    }
    
    // -------------------------------------------------------
    // Checking for Error condition
    // -------------------------------------------------------

    // Last Will Error
    if(last_will==1) {
        // delay of one second
        unsigned int delay_usec = 1*1000000;
        
        // set to 100 by default
        motorway_speed=100;
        
        // flash LED to indicate fail
        for(int x=0; x<LWNUMLEDFLASHES; x++){
            turnOnLed();
            usleep(delay_usec);
            turnOffLed();
            usleep(delay_usec);
        }
    } 
    else {
        // update array of samples
        updateNumberOfVehiclesPassed(data);
        int newAverage = updateMotorWaySpeed();
        
        // check if an accident has occured
        if(newAverage<=0){
            turnOnLed();
        }
        // turn off if the accident has passed
        else{
            turnOffLed();
        }
    }
    
    // -------------------------------------------------------
    // Free up messages and clearing strings
    // -------------------------------------------------------
    
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    mqtt_message="";
    mqtt_message2="";
    mqtt_message3="";
    
    return 1;
}

void connlost(void *context, char *cause) {
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int main(int argc, char* argv[]) {
    MQTTClient client;
    MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
    int rc;
    int ch;
    
    // ---------------------------------------
    //  Setup of Motorway
    // ---------------------------------------
    
    // default of 10 cars starting off
    std::fill(numberOfVehiclesPassed, numberOfVehiclesPassed + SAMPLE_SIZE, DEFAULT_NUM_OF_VEHICLES);
    cout << "Current motorway speed: " << motorway_speed << endl;
    

    // ---------------------------------------
    // Create MQTT Subscriber
    // ---------------------------------------
    
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    opts.keepAliveInterval = 20;
    opts.cleansession = 1;
    opts.username = AUTHMETHOD;
    opts.password = AUTHTOKEN;

    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
    if ((rc = MQTTClient_connect(client, &opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(-1);
    }
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC, QOS);

    // --------------------------------------
    // Receiving Topic
    // --------------------------------------
    
    do {
        ch = getchar();
    } while(ch!='Q' && ch != 'q');
    
    // Disconnecting subscriber
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}
