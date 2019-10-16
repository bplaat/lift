//This code is written by Jaco de Jong
#include "digit_display.h"
#include "communication.h"
#include <stdint.h>
#include <Arduino.h>

#define TEST

#define LED 9
#define BUTTON_DOWN A0
#define BUTTON_DOWN_LED A1
#define BUTTON_UP A2
#define BUTTON_UP_LED A3
#define REED 10

void set_display(uint8_t digit)
{
    static bool knipper_state = false;
    static long millis_start = millis();
    if (recieved_action != MOVING)
    {
        writeDigit(digit);
    }
    else if (knipper_state == false)
    {
        writeDigit(digit);
        if (millis_start + 1000 < millis())
        {
            knipper_state = true;
            millis_start = millis();
        }
    }
    else if (knipper_state == true)
    {
        writeDigit(10);
        if (millis_start + 1000 < millis())
        {
            knipper_state = false;
            millis_start = millis();
        }
    }
}

void setup_IO()
{
    pinMode(BUTTON_DOWN, INPUT_PULLUP);
    pinMode(BUTTON_UP, INPUT_PULLUP);
    pinMode(BUTTON_DOWN_LED, OUTPUT);
    pinMode(BUTTON_UP_LED, OUTPUT);
    pinMode(REED, INPUT);
    pinMode(LED, OUTPUT);
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
}

void setup()
{
    setup_IO();
    setup_I2C();
    Serial.begin(9600);
    Serial.println("JACO_ETAGE is online");

#ifdef TEST
    Serial.println("testing........");
    digitalWrite(LED, LOW);
    digitalWrite(BUTTON_UP_LED, LOW);
    digitalWrite(BUTTON_DOWN_LED, LOW);

    for (uint8_t i = 0; i <= 9; i++)
    {
        writeDigit(i);
        delay(1000);
        //https: //www.allaboutcircuits.com/projects/interface-a-seven-segment-display-to-an-arduino/
    }

    digitalWrite(LED, LOW);
    digitalWrite(BUTTON_UP_LED, HIGH);
    digitalWrite(BUTTON_DOWN_LED, HIGH);
#endif
}

void loop()
{
    //writeDigit(recieved_floor);
    set_display(recieved_floor);
    if (digitalRead(BUTTON_DOWN))
    {
        send_stop = STOP_FOR_DOWN;
        //    Serial.println("down-button pressd");
    }
    else if (digitalRead(BUTTON_UP))
    {
        send_stop = STOP_FOR_UP;
        Serial.println("up-button pressed");
    }
    else
    {
        send_stop = NO_STOP_NEEDED;
        //Serial.println("no stop needed");
    }
    send_is_lift_here = digitalRead(REED);
    //Serial.println("lift is: " + send_is_lift_here);

    digitalWrite(LED, send_is_lift_here);
    Serial.println(recieved_floor);
    if (recieved_stop_accepted == STOP_FOR_DOWN_ACCEPTED)
    {
        digitalWrite(BUTTON_DOWN_LED, LOW);
    }
    if (recieved_stop_accepted == STOP_FOR_UP_ACCEPTED)
    {
        digitalWrite(BUTTON_UP_LED, LOW);
    }
}
