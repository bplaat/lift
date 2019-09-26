// ############################
// ### Bastiaans Lift Etage ###
// ############################
// Made by Bastiaan van der Plaat

#include "digit_display.h"

DigitDisplay *digit_display;
uint8_t digit_display_pins[7] = { 2, 3, 4, 5, 6, 7, 8 };

#define UP_BUTTON_PIN 9
bool upButtonDown = false;

#define DOWN_BUTTON_PIN 10
bool downButtonDown = false;

#define LED_PIN 11

void setup() {
    Serial.begin(9600);
    Serial.println("Bastiaan's Etage");
    digit_display = digit_display_new(digit_display_pins);
    pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
    pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    int upButtonState = digitalRead(UP_BUTTON_PIN);
    if (upButtonState == LOW && !upButtonDown) {
        upButtonDown = true;
        digit_display_set_digit(digit_display, 0xa);
        digitalWrite(LED_PIN, HIGH);
    }
    if (upButtonState == HIGH) {
        upButtonDown = false;
    }

    int downButtonState = digitalRead(DOWN_BUTTON_PIN);
    if (downButtonState == LOW && !downButtonDown) {
        downButtonDown = true;
        digit_display_set_digit(digit_display, 0xb);
        digitalWrite(LED_PIN, LOW);
    }
    if (downButtonState == HIGH) {
        downButtonDown = false;
    }
}
