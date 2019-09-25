#include <Wire.h>
#include "addresses.h"

#define SDA A4
#define SCL A5
#define DISPLAY0 2
#define DISPLAY1 3
#define DISPLAY2 4
#define DISPLAY3 5
#define DISPLAY4 6
#define DISPLAY5 7
#define DISPLAY6 8
#define LED 9
#define BUTTON1 A0
#define BUTTON1LED A1
#define BUTTON2 A3
#define BUTTON2LED A4
#define REED 10

void setup_IO()
{
    pinMode(BUTTON1, INPUT);
    pinMode(BUTTON2, INPUT);
    pinMode(REED, INPUT);

    pinMode(DISPLAY0, OUTPUT);
    pinMode(DISPLAY1, OUTPUT);
    pinMode(DISPLAY2, OUTPUT);
    pinMode(DISPLAY3, OUTPUT);
    pinMode(DISPLAY4, OUTPUT);
    pinMode(DISPLAY5, OUTPUT);
    pinMode(DISPLAY6, OUTPUT);
    pinMode(LED, OUTPUT);
    pinMode(BUTTON1LED, OUTPUT);
    pinMode(BUTTON2LED, OUTPUT);
}

void setup_I2C()
{
    Wire.begin(JACO_ETAGE);       // join i2c bus with address #8
    Wire.onReceive(receiveEvent); // register event
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent()
{
    while (1 < Wire.available())
    {                         // loop through all but the last
        char c = Wire.read(); // receive byte as a character
        Serial.print(c);      // print the character
    }
    int x = Wire.read(); // receive byte as an integer
    Serial.println(x);   // print the integer
}

void setup()
{
    setup_IO();
    setup_I2C();
}

void loop()
{
    
}
