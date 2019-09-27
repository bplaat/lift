//This code is written by Jaco de Jong
#include <Wire.h>
#include "addresses.h"
#include <Arduino.h>

#pragma once

#define NO_MOVING 0
#define DIRECTION_DOWN 1
#define DIRECTION_UP 2
#define NO_STOP_NEEDED 0
#define STOP_FOR_DOWN 1
#define STOP_FOR_UP 2
#define LIFT_IS_HERE 1
#define LIFT_IS_NOT_HERE 0
#define STOP_ACCEPTED 1
#define NO_STOP_ACCEPTED 0

uint8_t send_is_lift_here = LIFT_IS_NOT_HERE;
uint8_t send_stop = NO_STOP_NEEDED;

uint8_t recieved_floor = 0;
uint8_t recieved_direction = NO_MOVING;
uint8_t recieved_is_requested_stop_accepted = NO_STOP_ACCEPTED;

void receiveEvent()
{
}
void requestEvent()
{
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
