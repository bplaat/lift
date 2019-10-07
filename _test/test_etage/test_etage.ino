#include <Wire.h>

#define DEBUG

#define ETAGE_ADDRESS 1

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

#define REED_SENSOR_PIN A0

uint8_t lift_cabine_etage = 0;
int8_t lift_cabine_direction = 0;
uint8_t lift_cabine_is_here = 0;
int8_t lift_request_stop = 0;
int8_t lift_stop_accepted = 0;

#define BLINK_TIME 200
uint8_t blink_state = 0;
uint32_t blink_time = millis();

void setup() {
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println("Test Etage");
  #endif

  Wire.begin(ETAGE_ADDRESS);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

  digit_display_init();
  pinMode(LED_PIN, OUTPUT);
  pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(UP_LED_PIN, OUTPUT);
  pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DOWN_LED_PIN, OUTPUT);
  pinMode(REED_SENSOR_PIN, INPUT_PULLUP);
}

void loop() {
  lift_cabine_is_here = 1 - digitalRead(REED_SENSOR_PIN);
  digitalWrite(LED_PIN, lift_cabine_is_here);

  if (lift_cabine_direction == 0) {
    digit_display_set_digit(lift_cabine_etage);
  } else {
    if (blink_state == 0 && millis() - blink_time > BLINK_TIME) {
      blink_state = 1;
      blink_time = millis();
    }
    if (blink_state == 1 && millis() - blink_time > BLINK_TIME) {
      blink_state = 0;
      blink_time = millis();
    }

    if (blink_state) {
      digit_display_set_digit(lift_cabine_etage);
    } else {
      digit_display_set_bits(0);
    }
  }

  digitalWrite(UP_LED_PIN, lift_stop_accepted == 1);
  if (digitalRead(UP_BUTTON_PIN) == LOW) {
    if (!upButtonDown) {
      upButtonDown = true;
      lift_request_stop = 1;
    }
  } else {
    upButtonDown = false;
  }

  digitalWrite(DOWN_LED_PIN, lift_stop_accepted == -1);
  if (digitalRead(DOWN_BUTTON_PIN) == LOW) {
    if (!downButtonDown) {
      downButtonDown = true;
      lift_request_stop = -1;
    }
  } else {
    downButtonDown = false;
  }
}

void requestEvent() {
  Wire.write(lift_cabine_is_here);
  #ifdef DEBUG
    Serial.print("<- lift_cabine_is_here = ");
    Serial.println(lift_cabine_is_here);
  #endif

  Wire.write(lift_request_stop);
  #ifdef DEBUG
    Serial.print("<- lift_request_stop = ");
    Serial.println(lift_request_stop);
  #endif
  lift_request_stop = 0;
  lift_stop_accepted = 0;
}

void receiveEvent() {
  lift_cabine_etage = Wire.read();
  #ifdef DEBUG
    Serial.print("-> lift_cabine_etage = ");
    Serial.println(lift_cabine_etage);
  #endif

  lift_cabine_direction = Wire.read();
  #ifdef DEBUG
    Serial.print("-> lift_cabine_direction = ");
    Serial.println(lift_cabine_direction);
  #endif

  int8_t new_lift_stop_accepted = Wire.read();
  if (new_lift_stop_accepted != 0) {
    lift_stop_accepted = new_lift_stop_accepted;
    #ifdef DEBUG
      Serial.print("-> lift_stop_accepted = ");
      Serial.println(lift_cabine_direction);
    #endif
  }
}
