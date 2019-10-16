
#pragma once
#include <Arduino.h>
#define clockPin 10 // pin connected to SRCLK of 74HC595
#define dataPin 11  // pin connected to SER of 74HC595
#define latchPin 12 // pin connected to RCLK of 74HC595

const byte datArray[] = {
    B10000001, // 0
    B11110011, // 1
    B01001001, // 2
    B01100001, // 3
    B00110011, // 4
    B00100101, // 5
    B00000101, // 6
    B11110001, // 7
    B00000001, // 8
    B00100001, // 9
    B11111111};

void writeDigit(int i)
{
    digitalWrite(latchPin, LOW);                         // latchPin low for duration of transmission
    shiftOut(dataPin, clockPin, MSBFIRST, ~datArray[i]); // send data
    digitalWrite(latchPin, HIGH);                        // latchPin high to save the data
}
