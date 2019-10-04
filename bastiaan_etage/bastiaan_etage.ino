// ############################
// ### Bastiaans Lift Etage ###
// ############################
// Made by Bastiaan van der Plaat

// ############ Digit display ############

// The digit display pins
uint8_t digit_display_pins[7] = { 2, 3, 4, 5, 6, 7, 8 };

// The digit display digits
uint8_t digit_display_digits[16] = {
    0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
    0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111,
    0b01110111, 0b01111100, 0b00111001, 0b01011110, 0b01111001, 0b01110001
};

// Function that inits the digit display pins
void digit_display_init(void) {
    for (uint8_t i = 0; i < 7; i++) {
        pinMode(digit_display_pins[i], OUTPUT);
    }
}

// Function that sets the leds in a digit display
void digit_display_set_bits(uint8_t bits) {
    for (uint8_t i = 0; i < 7; i++) {
        digitalWrite(digit_display_pins[i], (bits >> i) & 1);
    }
}

// Function that displays a hex digit to the display
void digit_display_set_digit(uint8_t digit) {
    if (digit <= 15) {
        digit_display_set_bits(digit_display_digits[digit]);
    }
}

// Led
#define LED_PIN 9

// Up button
#define UP_BUTTON_PIN 10
#define UP_LED_PIN 11
bool upButtonDown = false;

// Down button
#define DOWN_BUTTON_PIN 12
#define DOWN_LED_PIN 13
bool downButtonDown = false;

// IR Sensor
#define IR_SENSOR_PIN A0

// Adresses
#define BASTIAAN_ETAGE 1
#define DENIZ_ETAGE 2
#define JACO_ETAGE 3
#define LANGPEI_ETAGE 4
#define MAHMOUD_ETAGE 5
#define ROB_ETAGE 6

// Setup
void setup() {
    // Init serial
    Serial.begin(9600);
    Serial.println("Bastiaan's Etage");

    // Init pins
    digit_display_init();
    pinMode(LED_PIN, OUTPUT);
    pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
    pinMode(UP_LED_PIN, OUTPUT);
    pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
    pinMode(DOWN_LED_PIN, OUTPUT);
    pinMode(IR_SENSOR_PIN, INPUT);
}

// Loop
void loop() {
    // Check up button
    if (digitalRead(UP_BUTTON_PIN) == LOW) {
        if (!upButtonDown) {
            upButtonDown = true;
        }
    } else {
        upButtonDown = false;
    }

    // Check down button
    if (digitalRead(DOWN_BUTTON_PIN) == LOW) {
        if (!downButtonDown) {
            downButtonDown = true;
        }
    } else {
        downButtonDown = false;
    }

    // Read sensor
    if (analogRead(IR_SENSOR_PIN) > 1000) {
        // The lift is here
    }
}
