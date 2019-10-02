//This code is written by Jaco de Jong
#pragma once

#include <Wire.h>
#include "addresses.h"
#include <Arduino.h>

#define NO_MOVING 0
#define DIRECTION_DOWN 1
#define DIRECTION_UP 2

#define NO_STOP_NEEDED 0
#define STOP_FOR_DOWN 1
#define STOP_FOR_UP 2

#define NO_STOP_ACCEPTED 0
#define STOP_FOR_DOWN_ACCEPTED 1
#define STOP_FOR_UP_ACCEPTED 2
#define LiFT_IS_ARRIVED 3

#define LIFT_IS_HERE 1
#define LIFT_IS_NOT_HERE 0

uint8_t send_is_lift_here = LIFT_IS_NOT_HERE;
uint8_t send_stop = NO_STOP_NEEDED;

uint8_t recieved_floor = 0;
uint8_t recieved_direction = NO_MOVING;
uint8_t recieved_stop_accepted = NO_STOP_ACCEPTED;

void receiveEvent()
{
    recieved_floor = Wire.read();
    recieved_direction = Wire.read();
    recieved_stop_accepted = Wire.read();
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

void receiveEventForAnsiCharacters()
{
    while (1 < Wire.available())
    {
        char c = Wire.read();
        Serial.print(c);
    }
    char recieved_byte = Wire.read();
    Serial.print(recieved_byte);
}
