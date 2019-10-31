// ############################
// ### Bastiaans Lift Etage ###
// ############################
// Made by Bastiaan van der Plaat

// Include the wire library for I2C
#include <Wire.h>

// Enable the DEBUG flag
#define DEBUG

// The etage address and other protocol constants
#define ETAGE_ADDRESS 1
#define PROTOCOL_RECEIVE_MESSAGE_LENGTH 3

// The pins for the digit display
uint8_t digit_display_pins[7] = { 2, 3, 4, 5, 6, 7, 8 };

// The digits for the digit display
uint8_t digit_display_digits[10] = {
  0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
  0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111
};

// Function that inits the digit display
void digit_display_init() {
  for (uint8_t i = 0; i < 7; i++) {
    pinMode(digit_display_pins[i], OUTPUT);
  }
}

// Function that sets the bits of a digit display
void digit_display_set_bits(uint8_t bits) {
  for (uint8_t i = 0; i < 7; i++) {
    digitalWrite(digit_display_pins[i], (bits >> i) & 1);
  }
}

// Function that set a digit on the digit display
void digit_display_set_digit(uint8_t digit) {
  if (digit <= 9) {
    digit_display_set_bits(digit_display_digits[digit]);
  }
}

// Other pins
#define LED_PIN 9

#define UP_BUTTON_PIN 10
#define UP_LED_PIN 11

#define DOWN_BUTTON_PIN 12
#define DOWN_LED_PIN 13

#define IR_SENSOR_PIN A0

// Global lift variables
#define UP 1
#define DOWN -1

#define LIFT_STATE_STILL 0
#define LIFT_STATE_MOVING 1
#define LIFT_STATE_WAITING 2

uint8_t lift_etage = 0;
uint8_t lift_state = 0;
uint8_t lift_is_here = 0;
int8_t lift_request_stop = 0;
int8_t lift_stop_accepted = 0;

#define BLINK_TIME 200
#define BLINK_STATE_OFF 0
#define BLINK_STATE_ON 1

uint8_t blink_state = 0;
uint32_t blink_time = millis();

// On I2C receive read data
void receiveEvent(int16_t num_bytes) {
  if (num_bytes == PROTOCOL_RECEIVE_MESSAGE_LENGTH) {
    lift_etage = Wire.read();
    #ifdef DEBUG
      Serial.print("-> lift_etage = ");
      Serial.println(lift_etage);
    #endif

    lift_state = Wire.read();
    #ifdef DEBUG
      Serial.print("-> lift_state = ");
      Serial.println(lift_state);
    #endif

    int8_t new_lift_stop_accepted = Wire.read();
    if (new_lift_stop_accepted != 0) {
      lift_stop_accepted = new_lift_stop_accepted;
      #ifdef DEBUG
        Serial.print("-> lift_stop_accepted = ");
        Serial.println(lift_stop_accepted);
      #endif
    }
  }
  #ifdef DEBUG
    else {
      Serial.println("ERROR: Received wrong amount off bytes!");
    }
  #endif
}

// On I2C request write data
void requestEvent() {
  Wire.write(1);
  #ifdef DEBUG
    Serial.println("<- ping = 1");
  #endif

  Wire.write(lift_is_here);
  #ifdef DEBUG
    Serial.print("<- lift_is_here = ");
    Serial.println(lift_is_here);
  #endif

  Wire.write(lift_request_stop);
  #ifdef DEBUG
    Serial.print("<- lift_request_stop = ");
    Serial.println(lift_request_stop);
  #endif
  if (lift_request_stop != 0) {
    lift_request_stop = 0;
  }
}

void setup() {
  // When in debug mode is enabled init the serial communication
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println("Bastiaan's Etage");
  #endif

  // Init the I2C functions
  Wire.begin(ETAGE_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  // Init the other stuff
  digit_display_init();
  pinMode(LED_PIN, OUTPUT);
  pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(UP_LED_PIN, OUTPUT);
  pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DOWN_LED_PIN, OUTPUT);
  pinMode(IR_SENSOR_PIN, INPUT);
}

void loop() {
  // Check if lift cabine is here
  lift_is_here = analogRead(IR_SENSOR_PIN) < 50;
  digitalWrite(LED_PIN, lift_state != LIFT_STATE_MOVING && lift_is_here);

  // Clear the stop accepted when this lift is waiting at my etage
  if (lift_state == LIFT_STATE_WAITING && lift_is_here) {
    lift_stop_accepted = 0;
  }

  // When the lift is moving blink the digit display
  if (lift_state == LIFT_STATE_MOVING) {
    if (blink_state == BLINK_STATE_OFF && millis() - blink_time > BLINK_TIME) {
      blink_state = BLINK_STATE_ON;
      blink_time = millis();
    }
    if (blink_state == BLINK_STATE_ON && millis() - blink_time > BLINK_TIME) {
      blink_state = BLINK_STATE_OFF;
      blink_time = millis();
    }

    if (blink_state == BLINK_STATE_ON) {
      digit_display_set_digit(lift_etage);
    } else {
      digit_display_set_bits(0);
    }
  } else {
    digit_display_set_digit(lift_etage);
  }

  // Handle stop up button
  digitalWrite(UP_LED_PIN, lift_stop_accepted == UP);
  if (
    digitalRead(UP_BUTTON_PIN) == LOW &&
    !(lift_state == LIFT_STATE_WAITING && lift_is_here) &&
    lift_stop_accepted == 0 && lift_request_stop == 0
  ) {
    lift_request_stop = UP;
  }

  // Handle stop down button
  digitalWrite(DOWN_LED_PIN, lift_stop_accepted == DOWN);
  if (
    digitalRead(DOWN_BUTTON_PIN) == LOW &&
    !(lift_state != LIFT_STATE_WAITING && lift_is_here) &&
    lift_stop_accepted == 0 && lift_request_stop == 0
  ) {
    lift_request_stop = DOWN;
  }
}
