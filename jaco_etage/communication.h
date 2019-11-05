
//This code is written by Jaco de Jong
#pragma once

#include <Wire.h>

#include <Arduino.h>

#define NO_MOVING 0
#define MOVING 1
#define WAITING 2

#define NO_STOP_NEEDED 0
#define STOP_FOR_DOWN 1
#define STOP_FOR_UP 2

#define STOP_FOR_DOWN_ACCEPTED -1
#define STOP_FOR_UP_ACCEPTED 1

#define LIFT_IS_HERE 1
#define JACO_ETAGE 1
#define ADDRESS_OFFSET 10

uint8_t send_is_lift_here = 0;
uint8_t send_stop_request = 0;
bool lift_stop_send = 0;

uint8_t recieved_floor = 0;
uint8_t recieved_action = WAITING;
bool recieved_stop_accepted_for_up = 0; //STOP_FOR_UP_ACCEPTED; //NO_STOP_ACCEPTED;
bool recieved_stop_accepted_for_down = 0;

// Recieve event hander for I2C
void receiveEvent()
{
    recieved_floor = Wire.read();
    recieved_action = Wire.read();
    int8_t recieved_stop_accepted = Wire.read();
    if (recieved_stop_accepted == STOP_FOR_DOWN_ACCEPTED)
    {
        recieved_stop_accepted_for_down = HIGH;
    }
    if (recieved_stop_accepted == STOP_FOR_UP_ACCEPTED)
    {
        recieved_stop_accepted_for_up = HIGH;
        Serial.println("checking in comm");
    }
}

// Request event hander for I2C
void requestEvent()
{

    Wire.write(1);
    Wire.write(send_is_lift_here);
    Wire.write(send_stop_request);
    if (send_stop_request != 0)
    {
        send_stop_request = 0;
        lift_stop_send = true;
    }
    send_stop_request = 0;
}

// set the I2C up.
void setup_I2C()
{
    Wire.begin(JACO_ETAGE + ADDRESS_OFFSET);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
}