// ############################
// ### Bastiaans Lift Etage ###
// ############################
// Made by Bastiaan van der Plaat

#include "digit_display.h"

uint8_t counter = 0;

DigitDisplay *digit_display;
uint8_t digit_display_pins[7] = { 2, 3, 4, 5, 6, 7, 8 };

void setup() {
    Serial.begin(9600);
    Serial.println("Bastiaan's Etage");
    digit_display = digit_display_new(digit_display_pins);
}

void loop() {
    digit_display_set_digit(digit_display, counter);
    if (counter == 15) {
        counter = 0;
    } else {
        counter++;
    }
    delay(250);
}
