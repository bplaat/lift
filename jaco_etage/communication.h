
//This code is written by Jaco de Jong
#pragma once

#include <Wire.h>
#include "addresses.h"
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

uint8_t send_is_lift_here = 0;
uint8_t send_stop = 0;

uint8_t recieved_floor = 1;
uint8_t recieved_action = NO_MOVING;
bool recieved_stop_accepted_for_up = 0; //STOP_FOR_UP_ACCEPTED; //NO_STOP_ACCEPTED;
bool recieved_stop_accepted_for_down = 0;

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
void requestEvent()
{
    Wire.write(1);
    Wire.write(send_is_lift_here);
    Wire.write(send_stop);
}

void setup_I2C()
{
    Wire.begin(JACO_ETAGE);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);
}