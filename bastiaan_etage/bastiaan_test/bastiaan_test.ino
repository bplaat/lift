// #################################
// ### Bastiaans Lift Etage Test ###
// #################################
// Made by Bastiaan van der Plaat

uint8_t digit_display_pins[7] = { 2, 3, 4, 5, 6, 7, 8 };

uint8_t digit_display_digits[10] = {
    0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
    0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111
};

void digit_display_init() {
    for (uint8_t i = 0; i < 7; i++) {
        pinMode(digit_display_pins[i], OUTPUT);
    }
}

void digit_display_set_bits(uint8_t bits) {
    for (uint8_t i = 0; i < 7; i++) {
        digitalWrite(digit_display_pins[i], (bits >> i) & 1);
    }
}

void digit_display_set_digit(uint8_t digit) {
    if (digit <= 9) {
        digit_display_set_bits(digit_display_digits[digit]);
    }
}

#define LED_PIN 9

#define UP_BUTTON_PIN 10
#define UP_LED_PIN 11
bool up_pressed = false;

#define DOWN_BUTTON_PIN 12
#define DOWN_LED_PIN 13
bool down_pressed = false;

#define IR_SENSOR_PIN A0

#define TIMER_TIME 1000
uint32_t timer_time = millis();
uint8_t counter = 0;

void setup() {
    digit_display_init();
    pinMode(LED_PIN, OUTPUT);
    pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
    pinMode(UP_LED_PIN, OUTPUT);
    pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
    pinMode(DOWN_LED_PIN, OUTPUT);
    pinMode(IR_SENSOR_PIN, INPUT);
}

void loop() {
    digitalWrite(LED_PIN, analogRead(IR_SENSOR_PIN) < 50);

    if (millis() - timer_time > TIMER_TIME) {
        timer_time = millis();
        digit_display_set_digit(counter);
        if (counter == 9) {
            counter = 0;
        } else {
            counter++;
        }
    }

    digitalWrite(UP_LED_PIN, up_pressed);
    if (digitalRead(UP_BUTTON_PIN) == LOW) {
        up_pressed = !up_pressed;
    }

    digitalWrite(DOWN_LED_PIN, down_pressed);
    if (digitalRead(DOWN_BUTTON_PIN) == LOW) {
        down_pressed = !down_pressed;
    }
}
