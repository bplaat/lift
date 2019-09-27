// #############################################
// ### Digit Display Library for the Arduino ###
// #############################################
// Made by Bastiaan van der Plaat

#pragma once
#include <Arduino.h>
typedef struct DigitDisplay
{
    uint8_t *pins;
} DigitDisplay;

DigitDisplay *digit_display_new(uint8_t *pins)
{
    DigitDisplay *digit_display = malloc(sizeof(DigitDisplay));
    digit_display->pins = pins;
    for (uint8_t i = 0; i < 7; i++)
    {
        pinMode(digit_display->pins[i], OUTPUT);
    }
    return digit_display;
}

void digit_display_set_bits(DigitDisplay *digit_display, uint8_t bits)
{
    for (uint8_t i = 0; i < 7; i++)
    {
        digitalWrite(digit_display->pins[i], (bits >> i) & 1);
    }
}

uint8_t digit_display_digits[16] = {
    0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
    0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111,
    0b01110111, 0b01111100, 0b00111001, 0b01011110, 0b01111001, 0b01110001};

void digit_display_set_digit(DigitDisplay *digit_display, uint8_t digit)
{
    if (digit >= 0 && digit <= 15)
    {
        digit_display_set_bits(digit_display, digit_display_digits[digit]);
    }
}
