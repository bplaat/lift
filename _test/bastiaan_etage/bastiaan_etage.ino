#include <Wire.h>

#define DEBUG

uint8_t digit_display_pins[7] = { 2, 3, 4, 5, 6, 7, 8 };

uint8_t digit_display_digits[16] = {
    0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
    0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111,
    0b01110111, 0b01111100, 0b00111001, 0b01011110, 0b01111001, 0b01110001
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
    if (digit <= 15) {
        digit_display_set_bits(digit_display_digits[digit]);
    }
}

#define LED_PIN 9

#define UP_BUTTON_PIN 10
#define UP_LED_PIN 11
bool upButtonDown = false;

#define DOWN_BUTTON_PIN 12
#define DOWN_LED_PIN 13
bool downButtonDown = false;

#define IR_SENSOR_PIN A0

uint8_t lift_cabine_etage = 0;
uint8_t lift_cabine_direction = 0;
uint8_t lift_cabine_is_here = 0;

void setup() {
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println("Bastiaan's Etage");
  #endif

  Wire.begin(1);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

  digit_display_init();
  pinMode(LED_PIN, OUTPUT);
  pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(UP_LED_PIN, OUTPUT);
  pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DOWN_LED_PIN, OUTPUT);
  pinMode(IR_SENSOR_PIN, INPUT);
}

void loop() {
  lift_cabine_is_here = analogRead(IR_SENSOR_PIN) < 50;
  digitalWrite(LED_PIN, lift_cabine_is_here);

  digit_display_set_digit(lift_cabine_etage);

  if (digitalRead(UP_BUTTON_PIN) == LOW) {
    if (!upButtonDown) {
      upButtonDown = true;
      #ifdef DEBUG
        Serial.println("Request up stop");
      #endif
    }
  } else {
    upButtonDown = false;
  }

  if (digitalRead(DOWN_BUTTON_PIN) == LOW) {
    if (!downButtonDown) {
      downButtonDown = true;
      #ifdef DEBUG
        Serial.println("Request down stop");
      #endif
    }
  } else {
    downButtonDown = false;
  }
}

void requestEvent() {
  Wire.write(lift_cabine_is_here);
  #ifdef DEBUG
    Serial.print("lift_cabine_is_here = ");
    Serial.println(lift_cabine_is_here);
  #endif
}

void receiveEvent(int numBytes) {
  if (Wire.available()) {
    lift_cabine_etage = Wire.read();
    lift_cabine_direction = Wire.read();
    #ifdef DEBUG
      Serial.print("lift_cabine_etage = ");
      Serial.println(lift_cabine_etage);
      Serial.print("lift_cabine_direction = ");
      Serial.println(lift_cabine_direction);
    #endif
  }
}
