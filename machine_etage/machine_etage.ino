// ############################
// #### Machine Lift Etage ####
// ############################

// Include the wire library for I2C
#include <Wire.h>

// Include the keypad library for the keypad input
#include <Keypad.h>

// Enable the DEBUG flag
#define DEBUG

// Keypad vars
#define KEYPAD_ROWS 4
#define KEYPAD_COLUMNS 4

char keypad_keys[KEYPAD_ROWS][KEYPAD_COLUMNS]= {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

uint8_t keypad_row_pins[KEYPAD_ROWS] = { 2, 3, 4, 5 };
uint8_t keypad_column_pins[KEYPAD_COLUMNS] = { 6, 7, 8, 9 };

Keypad keypad = Keypad(makeKeymap(keypad_keys), keypad_row_pins, keypad_column_pins, KEYPAD_ROWS, KEYPAD_COLUMNS);

// Global lift variables
#define LIFT_ETAGES_COUNT 2
#define PROTOCOL_REQUEST_MESSAGE_LENGTH 3

#define UP 1
#define DOWN -1

#define LIFT_STATE_STILL 0
#define LIFT_STATE_MOVING 1
#define LIFT_STATE_WAITING 2

uint8_t lift_etage = 0;
uint8_t lift_state = LIFT_STATE_STILL;

// Lift input led pin
#define LED_PIN 10

// Motor pins
#define MOTOR_ENABLE_PIN 11
#define MOTOR_UP_PIN 12
#define MOTOR_DOWN_PIN 13

// Button pins
#define UP_BUTTON_PIN 22
#define DOWN_BUTTON_PIN 24

// The stop struct
typedef struct Stop {
  uint8_t etage;
  int8_t direction;
  uint8_t begin;
  uint8_t end;
  uint8_t reported;
} Stop;

// The stops array
#define STOPS_MAX_LENGTH 20
Stop *stops[STOPS_MAX_LENGTH];
uint8_t stops_length = 0;

// Vars for the end stop wait
#define WAIT_TIME 1500
#define WAIT_STATE_OFF 0
#define WAIT_STATE_ON 1
uint8_t wait_state = 0;
uint32_t wait_time = 0;

// Function that moves the motor up
void motor_up() {
  lift_state = LIFT_STATE_MOVING;
  digitalWrite(MOTOR_UP_PIN, HIGH);
  digitalWrite(MOTOR_DOWN_PIN, LOW);
  digitalWrite(MOTOR_ENABLE_PIN, HIGH);
}

// Function that moves the motor down
void motor_down() {
  lift_state = LIFT_STATE_MOVING;
  digitalWrite(MOTOR_UP_PIN, LOW);
  digitalWrite(MOTOR_DOWN_PIN, HIGH);
  digitalWrite(MOTOR_ENABLE_PIN, HIGH);
}

// Function that stops the motor
void motor_stop() {
  lift_state = LIFT_STATE_STILL;
  digitalWrite(MOTOR_UP_PIN, LOW);
  digitalWrite(MOTOR_DOWN_PIN, LOW);
  digitalWrite(MOTOR_ENABLE_PIN, LOW);
}

// Stops compare algorithm
// -1 = the stop is earlier
// 0 = don't move stop
// 1 = the stop is later
int16_t stops_compare(Stop *a, Stop *b) {
  // If the direction is different
  if (a->direction != b->direction) {
    // And a is an end stop and b is not move a earlier
    if (a->end && !b->end) {
      return -1;
    }

    // When b is an end and a is not move a later
    if (!a->end && b->end) {
      return 1;
    }
  }

  // When a is the same direction as the lift now is moving and b is not move a earlier
  if (a->direction == stops[0]->direction && b->direction != stops[0]->direction) {
    return -1;
  }

  // When b is the same direction as the lift is moving and a is not move a later
  if (a->direction != stops[0]->direction && b->direction == stops[0]->direction) {
    return 1;
  }

  // If a is closer to the current lift position then b move a earlier
  if (abs(lift_etage - a->etage) < abs(lift_etage - b->etage)) {
    return -1;
  }

  // If b is closer to the current lift position then a move b earlier
  if (abs(lift_etage - a->etage) > abs(lift_etage - b->etage)) {
    return 1;
  }

  // If you make it trough here a is on the right place
  return 0;
}

// Function that moves the lift cabine to the first stop etage
void goto_first_stop() {
  uint8_t direction = stops[0]->etage > lift_etage;

  #ifdef DEBUG
    Serial.print("Go from ");
    Serial.print(lift_etage);
    Serial.print(" to ");
    Serial.print(stops[0]->etage);
    Serial.print(" thats ");
    Serial.println(direction == 1 ? "up" : "down");
  #endif

  // Start the motor
  if (direction) {
    motor_up();
  } else {
    motor_down();
  }
}

// Function that updates the stops array with a new stop or edits a stop
void update_stops(uint8_t etage, int8_t direction, uint8_t end) {
  #ifdef DEBUG
    Serial.print("Add stop to ");
    Serial.print(etage);
    Serial.print(" with direction ");
    Serial.print(direction == 1 ? "up" : "down");
    Serial.print(" and is ");
    Serial.println(end == 0 ? "begin" : "end");
  #endif

  // Check if the stops array is not over written (This should never happen)
  if (stops_length >= STOPS_MAX_LENGTH) {
    #ifdef DEBUG
      Serial.println("ERROR: Max stops length!");
    #endif
    return;
  }

  // Check if the stop allready exists
  uint8_t already_stop = 0;
  for (uint8_t i = 0; i < stops_length; i++) {
    if (stops[i]->etage == etage) {
      already_stop = 1;

      // If this is an end stop set end flag
      if (end) {
        stops[i]->end = 1;
      }

      // Else update direction end begin and reported flags
      else {
        stops[i]->direction = direction;
        stops[i]->begin = 1;
        stops[i]->reported = 0;
      }

      break;
    }
  }

  // If the stop don't exits allocate a new stop and add it to the stops array
  if (!already_stop) {
    Stop *stop = (Stop *)malloc(sizeof(Stop));
    stop->etage = etage;
    stop->direction = direction;
    stop->begin = !end;
    stop->end = end;
    stop->reported = end;
    stops[stops_length++] = stop;
  }

  // Sort the stops array to find the best stop
  qsort(stops, stops_length, sizeof(Stop *), stops_compare);
}

void setup() {
  // When in debug mode is enabled init the serial communication
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println("Machine Etage");
  #endif

  // Init the I2C functions
  Wire.begin();

  // Init Arduino pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(MOTOR_ENABLE_PIN, OUTPUT);
  pinMode(MOTOR_UP_PIN, OUTPUT);
  pinMode(MOTOR_DOWN_PIN, OUTPUT);
  pinMode(UP_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DOWN_BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  // Check if there are no stops
  if (stops_length == 0) {
    // Check if up button is pressed and enable motor up
    if (digitalRead(UP_BUTTON_PIN) == LOW) {
      motor_up();
    }

    // Check if down button is pressed and enable motor down
    else if (digitalRead(DOWN_BUTTON_PIN) == LOW) {
      motor_down();
    }

    // Else disable motor
    else {
      motor_stop();
    }
  }

  // Check if the etage is at the first stop etage
  if (stops_length > 0 && lift_etage == stops[0]->etage) {
    motor_stop();

    // Check if first stop is not a begin stop but an end stop
    if (!stops[0]->begin && stops[0]->end) {
      // Set the lift on waiting a little time
      wait_state = WAIT_STATE_ON;
      wait_time = millis();
    } else {
      // Set the lift state to wait for user input
      lift_state = LIFT_STATE_WAITING;
    }

    // Free the first stop
    free(stops[0]);

    // Shift all the stop pointers one left
    for (uint8_t i = 1; i < stops_length; i++) {
      stops[i - 1] = stops[i];
    }

    // Decrement the stops array length
    stops_length--;
  }

  // Check of the stop end state is waiting: wait and then go to the next best stop
  if (wait_state == WAIT_STATE_ON && millis() - wait_time > WAIT_TIME) {
    // Remove the wait state
    wait_state = WAIT_STATE_OFF;

    // If there is another stop
    if (stops_length > 0) {
      // Resort the stops array
      qsort(stops, stops_length, sizeof(Stop *), stops_compare);

      // Go to the first stop
      goto_first_stop();
    }
  }

  // Check off the lift state is waiting
  digitalWrite(LED_PIN, lift_state == LIFT_STATE_WAITING);

  // If lift state is waiting
  if (lift_state == LIFT_STATE_WAITING) {
    // Get the pressed key number
    uint8_t etage = keypad.getKey() - '0';
    // If the number is between one and lift etages count and not on the same etage
    if (etage >= 1 && etage <= LIFT_ETAGES_COUNT && etage != lift_etage) {
      // Add new end stop
      update_stops(etage, etage > lift_etage ? UP : DOWN, true);

      // Go to first stop
      goto_first_stop();
    }
  }

  // Print debug lift state information that the master is going to send to all the slaves
  /*#ifdef DEBUG
    Serial.print("lift_etage = ");
    Serial.println(lift_etage);
    Serial.print("lift_state = ");
    Serial.println(lift_state);
  #endif*/

  // Loop over all the etages and send and receive information
  for (uint8_t etage = 1; etage <= LIFT_ETAGES_COUNT; etage++) {
    // Check if the stop accepted must be send
    uint8_t lift_stop_accepted = 0;
    for (uint8_t i = 0; i < stops_length; i++) {
      if (stops[i]->etage == etage && !stops[i]->reported) {
        lift_stop_accepted = stops[i]->direction;
        stops[i]->reported = 1;
        break;
      }
    }

    // Send the information conform protocol
    Wire.beginTransmission(etage);
    Wire.write(lift_etage);
    Wire.write(lift_state);
    Wire.write(lift_stop_accepted);
    Wire.endTransmission();

    // Request other information conform protocol
    if (Wire.requestFrom(etage, PROTOCOL_REQUEST_MESSAGE_LENGTH) == PROTOCOL_REQUEST_MESSAGE_LENGTH) {
      // Read ping value and ignore
      Wire.read();

      // Read lift is here value
      uint8_t lift_is_here = Wire.read();

      // If the lift is here update lift etage var
      if (lift_is_here == 1) {
        /* If the lift etage was zero stop the motor (for first etage search)
        if (lift_etage == 0) {
          motor_stop();
        }*/

        lift_etage = etage;
      }

      // Read request lift stop value
      int8_t lift_request_stop = Wire.read();

      // If there is a stop add the stop
      if (lift_request_stop != 0) {
          update_stops(etage, lift_request_stop, 0);
          // First the stop is first and the lift is not waiting go to the first
          if (stops[0]->etage == etage && lift_state != LIFT_STATE_WAITING) {
            goto_first_stop();
          }
      }
    }

    // If slave etage didn't respond with the right amount of bytes then this exception will handle it
    #ifdef DEBUG
      else {
        Serial.print("ERROR: Etage ");
        Serial.print(etage);
        Serial.println(" didn't respond the right amount of bytes!");
      }
    #endif
  }
}
