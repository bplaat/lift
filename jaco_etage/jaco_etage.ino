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

DigitDisplay *display;
uint8_t display_pins[7] = {2, 3, 4, 5, 6, 7, 8};

void setup_IO()
{
    pinMode(BUTTON_DOWN, OUTPUT);
    pinMode(BUTTON_UP, OUTPUT);
    pinMode(BUTTON_DOWN_LED, INPUT_PULLUP);
    pinMode(BUTTON_UP_LED, INPUT_PULLUP);
    pinMode(REED, INPUT);
    pinMode(LED, OUTPUT);
}

void setup()
{
    setup_IO();
    setup_I2C();
    Serial.begin(9600);
    Serial.println("JACO_ETAGE is online");
    display = digit_display_new(display_pins);

#ifdef TEST
    Serial.println("testing........");
    digitalWrite(LED, HIGH);
    digitalWrite(BUTTON_UP_LED, HIGH);
    digitalWrite(BUTTON_DOWN_LED, HIGH);

    for (uint8_t i = 0; i <= 9; i++)
    {
        digit_display_set_digit(display, i);
        delay(1000);
        //https: //www.allaboutcircuits.com/projects/interface-a-seven-segment-display-to-an-arduino/
    }

    digitalWrite(LED, LOW);
    digitalWrite(BUTTON_UP_LED, LOW);
    digitalWrite(BUTTON_DOWN_LED, LOW);
#endif
}

void loop()
{
    digit_display_set_digit(display, recieved_floor);
    if (digitalRead(BUTTON_DOWN))
    {
        send_stop = STOP_FOR_DOWN;
        Serial.println("down-button pressd");
    }
    else if (digitalRead(BUTTON_UP))
    {
        send_stop = STOP_FOR_UP;
        Serial.println("up-button pressed");
    }
    else
    {
        send_stop = NO_STOP_NEEDED;
        Serial.println("no stop needed");
    }
    send_is_lift_here = digitalRead(REED) ? LIFT_IS_HERE : LIFT_IS_NOT_HERE;
    Serial.println("lift is: " + send_is_lift_here);
    digitalWrite(LED, send_is_lift_here);
}
