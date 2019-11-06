/*Mahmoud Sakr
 klas: TI1E
 */



#include <Wire.h>

#define ETAGE_ADDRESS 5
#define LIFT_ETAGE_ADDRESS_OFFSET 10

#define A_DISPLAY_PIN 8                    // seven-seg pins...
#define B_DISPLAY_PIN 9
#define C_DISPLAY_PIN 4
#define D_DISPLAY_PIN 5
#define E_DISPLAY_PIN 6
#define F_DISPLAY_PIN 2
#define G_DISPLAY_PIN 3

#define LED_PIN A0                      //led

#define UP_BUTTON_PIN 10                // but-1
#define UP_LED_PIN 13

#define DOWN_BUTTON_PIN 12              //but-2
#define DOWN_LED_PIN 7

#define REED_PIN 11

#define UP 1
#define DOWN -1

#define LIFT_STATE_STILL 0                   //lift states
#define LIFT_STATE_MOVING 1
#define LIFT_STATE_WAITING 2

int lift_etage = 0;
int lift_state = 0;

int lift_is_here = 0;

int lift_stop_request = 0;
int lift_stop_send = 0;
int lift_stop_accepted = 0;

#define BLINK_TIME 100                    //while moving the led blinks
#define BLINK_STATE_OFF 0
#define BLINK_STATE_ON 1

int blink_state = 0;
unsigned long blink_time = millis();

void displayDigit(int digit) {
  if (digit != 1 && digit != 4) digitalWrite(A_DISPLAY_PIN, HIGH);                                    //seven segment states...
  if (digit != 5 && digit != 6) digitalWrite(B_DISPLAY_PIN, HIGH);
  if (digit != 2) digitalWrite(C_DISPLAY_PIN, HIGH);
  if (digit != 1 && digit != 4 && digit != 7) digitalWrite(D_DISPLAY_PIN, HIGH);
  if (digit == 2 || digit == 6 || digit == 8 || digit == 0) digitalWrite(E_DISPLAY_PIN, HIGH);
  if (digit != 1 && digit != 2 && digit != 3 && digit != 7) digitalWrite(F_DISPLAY_PIN, HIGH);
  if (digit != 0 && digit != 1 && digit != 7) digitalWrite(G_DISPLAY_PIN, HIGH);
}

void clearDigit() {
  digitalWrite(A_DISPLAY_PIN, LOW);
  digitalWrite(B_DISPLAY_PIN, LOW);
  digitalWrite(C_DISPLAY_PIN, LOW);
  digitalWrite(D_DISPLAY_PIN, LOW);
  digitalWrite(E_DISPLAY_PIN, LOW);
  digitalWrite(F_DISPLAY_PIN, LOW);
  digitalWrite(G_DISPLAY_PIN, LOW);
}

void receiveEvent() {
  lift_etage = Wire.read();
  lift_state = Wire.read();
  int new_lift_stop_accepted = Wire.read();
  if (new_lift_stop_accepted != 0) {
    lift_stop_accepted = new_lift_stop_accepted;
    lift_stop_send = 0;
  }
}

void requestEvent() {
  Wire.write(1);
  Wire.write(lift_is_here);
  Wire.write(lift_stop_request);
  if (lift_stop_request != 0) {
    lift_stop_request = 0;
    lift_stop_send = 1;
  }
}

void setup() {
  Wire.begin(ETAGE_ADDRESS + LIFT_ETAGE_ADDRESS_OFFSET);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  pinMode(A_DISPLAY_PIN, OUTPUT);
  pinMode(B_DISPLAY_PIN, OUTPUT);
  pinMode(C_DISPLAY_PIN, OUTPUT);
  pinMode(D_DISPLAY_PIN, OUTPUT);
  pinMode(E_DISPLAY_PIN, OUTPUT);
  pinMode(F_DISPLAY_PIN, OUTPUT);
  pinMode(G_DISPLAY_PIN, OUTPUT);

  pinMode(LED_PIN, OUTPUT);
  pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(UP_LED_PIN, OUTPUT);
  pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DOWN_LED_PIN, OUTPUT);
  pinMode(REED_PIN, INPUT_PULLUP);
}

void loop() {
  lift_is_here = digitalRead(REED_PIN) == LOW;

  if (lift_state != LIFT_STATE_MOVING && lift_is_here) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  if (lift_state == LIFT_STATE_WAITING && lift_is_here) {
    lift_stop_accepted = 0;
  }

  clearDigit();

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
      displayDigit(lift_etage);
    }
  } else {
    displayDigit(lift_etage);
  }

  if (lift_stop_accepted == UP) {
    digitalWrite(UP_LED_PIN, HIGH);
  } else {
    digitalWrite(UP_LED_PIN, LOW);
  }

  if (
    digitalRead(UP_BUTTON_PIN) == LOW &&
    !(lift_state == LIFT_STATE_WAITING && lift_is_here) &&
    lift_stop_accepted == 0 && lift_stop_send == 0 && lift_stop_request == 0
  ) {
    lift_stop_request = UP;
  }

  if (lift_stop_accepted == DOWN) {
    digitalWrite(DOWN_LED_PIN, HIGH);
  } else {
    digitalWrite(DOWN_LED_PIN, LOW);
  }

  if (
    digitalRead(DOWN_BUTTON_PIN) == LOW &&
    !(lift_state != LIFT_STATE_WAITING && lift_is_here) &&
    lift_stop_accepted == 0 && lift_stop_send == 0 && lift_stop_request == 0
  ) {
    lift_stop_request = DOWN;
  }
}
