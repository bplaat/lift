// ############################
// ### Bastiaans Lift Etage ###
// ############################
// Made by Bastiaan van der Plaat

#include "digit_display.h"

#define BUTTON_PIN 9
#define LED_PIN 10

uint8_t counter = 0;
bool button_pressed = false;

DigitDisplay *digit_display;
uint8_t digit_display_pins[7] = { 2, 3, 4, 5, 6, 7, 8 };

void setup() {
    Serial.begin(9600);
    Serial.println("Bastiaan's Etage");

    digit_display = digit_display_new(digit_display_pins);

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    digit_display_set_digit(digit_display, counter);

    if (digitalRead(BUTTON_PIN) == LOW) {
        if (!button_pressed) {
            button_pressed = true;
            digitalWrite(LED_PIN, HIGH);

            if (counter == 15) {
                counter = 0;
            } else {
                counter++;
            }
        }
    }
    else {
        button_pressed = false;
        digitalWrite(LED_PIN, LOW);
    }
}
