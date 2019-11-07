//This code is written by Jaco de Jong
#include "digit_display.h"
#include "communication.h"
#include <stdint.h>
#include <Arduino.h>

//#define TEST

#define LIFT_HERE_LED 9

#define BUTTON_DOWN A0
#define BUTTON_DOWN_LED A1
#define BUTTON_UP A2
#define BUTTON_UP_LED A3
#define REED 7
#define GROUND_PINS_COUNT 5
const uint8_t GROUND_PINS[] = {3, 4, 5, 6, 8};

// Set the display, knipper when the lift is moving.
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
        if (millis_start + 100 < millis())
        {
            knipper_state = true;
            millis_start = millis();
        }
    }
    else if (knipper_state == true)
    {
        writeDigit(10);
        if (millis_start + 100 < millis())
        {
            knipper_state = false;
            millis_start = millis();
        }
    }
}

// Set the IO up.
void setup_IO()
{
    pinMode(BUTTON_DOWN, INPUT_PULLUP);
    pinMode(BUTTON_UP, INPUT_PULLUP);
    pinMode(BUTTON_DOWN_LED, OUTPUT);
    pinMode(BUTTON_UP_LED, OUTPUT);
    pinMode(REED, INPUT_PULLUP);
    pinMode(LIFT_HERE_LED, OUTPUT);
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    digitalWrite(BUTTON_UP_LED, LOW);
    digitalWrite(BUTTON_DOWN_LED, LOW);
    for (uint8_t i = 0; i < GROUND_PINS_COUNT; i++)
    {
        pinMode(GROUND_PINS[i], OUTPUT);
        digitalWrite(GROUND_PINS[i], LOW);
    }
}

void setup()
{
    setup_IO();
    setup_I2C();
#ifdef TEST
    Serial.begin(9600);
    Serial.println("JACO_ETAGE is online");
#endif
#ifdef TEST
    Serial.println("testing........");
    digitalWrite(LIFT_HERE_LED, HIGH);
    digitalWrite(BUTTON_UP_LED, HIGH);
    digitalWrite(BUTTON_DOWN_LED, HIGH);

    for (uint8_t i = 0; i <= 9; i++)
    {
        writeDigit(i);
        delay(1000);
        //https: //www.allaboutcircuits.com/projects/interface-a-seven-segment-display-to-an-arduino/
    }

    digitalWrite(LIFT_HERE_LED, LOW);
    digitalWrite(BUTTON_UP_LED, LOW);
    digitalWrite(BUTTON_DOWN_LED, LOW);
#endif
}

void loop()
{
#ifdef TEST
    // Print floor when it changes.
    static uint8_t previous_recieved_floor;
    if (recieved_floor != previous_recieved_floor)
    {
        previous_recieved_floor = recieved_floor;
        Serial.print("recieved_floor: ");
        Serial.println(recieved_floor);
    }

    if (!digitalRead(BUTTON_UP))
    {
        digitalWrite(BUTTON_UP_LED, HIGH);
    }
    if (!digitalRead(BUTTON_DOWN))
    {
        digitalWrite(BUTTON_DOWN_LED, HIGH);
    }
#endif

    // Set the recieved floor number on the display
    set_display(recieved_floor);

    // Check if the buttons are pressed.
    if (!digitalRead(BUTTON_DOWN) && lift_stop_send == false && !(recieved_action == WAITING && send_is_lift_here))
    {
        send_stop_request = STOP_FOR_DOWN;
#ifdef TEST
        Serial.println("down-button pressed");
#endif
    }
    else if (!digitalRead(BUTTON_UP) && lift_stop_send == false && !(recieved_action == WAITING && send_is_lift_here))
    {
        send_stop_request = STOP_FOR_UP;
#ifdef TEST
        Serial.println("up-button pressed");
#endif
    }
    else
    {
        send_stop_request = NO_STOP_NEEDED;
#ifdef TEST
        Serial.println("no stop needed");
#endif
    }

    // read the reed sensor out.
    send_is_lift_here = !digitalRead(REED);
    //Serial.println("lift is: " + send_is_lift_here);

    //  Write the red led high when the lift is stopped here.
    digitalWrite(LIFT_HERE_LED, !digitalRead(REED) /*&& recieved_action == WAITING*/);

    // Write the led in the button high when the stop is accepted
    if (recieved_stop_accepted_for_down)
    {
        digitalWrite(BUTTON_DOWN_LED, HIGH);
#ifdef TEST
        Serial.println("stop for down accepted");
#endif
    }
    else
    {
        digitalWrite(BUTTON_DOWN_LED, LOW);
    }
    if (recieved_stop_accepted_for_up)
    {
        digitalWrite(BUTTON_UP_LED, HIGH);
#ifdef TEST
        Serial.println("stop for up accepted");
#endif
    }
    else
    {
        digitalWrite(BUTTON_UP_LED, LOW);
    }

    // Reset the stop accepted byte.
    if (recieved_action == WAITING && !digitalRead(REED))
    {
        recieved_stop_accepted_for_down = 0;
        recieved_stop_accepted_for_up = 0;
        lift_stop_send = false;
    }
}
