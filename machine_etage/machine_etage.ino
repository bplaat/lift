// ############################
// #### Machine Lift Etage ####
// ############################

// Include the wire library for I2C
#include <Wire.h>

// Include the keypad library for the input keypad
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
#define LIFT_ETAGES_COUNT 5
#define PROTOCOL_REQUEST_MESSAGE_LENGTH 3

#define UP 1
#define DOWN -1

#define LIFT_STATE_STILL 0
#define LIFT_STATE_MOVING 1
#define LIFT_STATE_WAITING 2

uint8_t lift_etage = 0;
uint8_t lift_state = LIFT_STATE_STILL;

// Lift input led pins
#define LED_PIN 10

// DC motor pin
#define ENABLE_PIN 11
#define DIRECTION_PIN 12

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
Stop *stops[STOPS_MAX_LENGTH] = { 0 };
uint8_t stops_length = 0;

// Function that removes the first stop from the stops array
void stops_shift() {
  free(stops[0]);
  for (uint8_t i = 1; i < stops_length; i++) {
    stops[i - 1] = stops[i];
  }
  stops_length--;
}

// Vars for the end stop wait
#define WAIT_TIME 1500
#define WAIT_STATE_OFF 0
#define WAIT_STATE_ON 1
uint8_t wait_state = 0;
uint32_t wait_time = 0;

// Stops compare algorithm
// -1 = stop is earlier
// 0 = don't move stop
// 1 = stop is later
int16_t stops_compare(Stop *a, Stop *b) {
  if (a->direction != b->direction) {
    if (a->end && !b->end) {
      return -1;
    }
    if (!a->end && b->end) {
      return 1;
    }
  }
  if (a->direction == stops[0]->direction && b->direction != stops[0]->direction) {
    return -1;
  }
  if (a->direction != stops[0]->direction && b->direction == stops[0]->direction) {
    return 1;
  }
  if (abs(lift_etage - a->etage) < abs(lift_etage - b->etage)) {
    return -1;
  }
  if (abs(lift_etage - a->etage) > abs(lift_etage - b->etage)) {
    return 1;
  }
  return 0;
}

// Function that handles a stop move end
void handle_stops_end () {
  lift_state = LIFT_STATE_STILL;
  if (stops_length > 0) {
    if (!stops[0]->begin && stops[0]->end) {
      stops_shift();
      wait_state = WAIT_STATE_ON;
      wait_time = millis();
    } else {
      stops_shift();
      lift_state = LIFT_STATE_WAITING;
    }
  }
}

// Function that moves the lift cabine to the first stop etage
void goto_first_stop() {
  // lift cabine move to stops[0]->etage
  
}

// Function that updates the stops array with a new stop or edits a stop
void update_stops(uint8_t etage, int8_t direction, uint8_t end) {
  if (stops_length >= STOPS_MAX_LENGTH) {
    #ifdef DEBUG
      Serial.println("ERROR: Max stops length!");
    #endif
    return;
  }

  uint8_t already_stop = 0;
  for (uint8_t i = 0; i < stops_length; i++) {
    if (stops[i]->etage == etage) {
      already_stop = 1;
      if (!end) {
        stops[i]->direction = direction;
        stops[i]->begin = 1;
        stops[i]->reported = 0;
      } else {
        stops[i]->end = 1;
      }
      break;
    }
  }

  if (!already_stop) {
    Stop *stop = (Stop *)malloc(sizeof(Stop));
    stop->etage = etage;
    stop->direction = direction;
    stop->begin = !end;
    stop->end = end;
    stop->reported = end;
    stops[stops_length++] = stop;
  }

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
  
  pinMode(LED_PIN);
  
  pinMode(DIRECTION_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);

  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(DIRECTION, LOW);

  Serial.println("Motor ready");
}

void loop() {
  // Check of the stop end state is waiting: wait and then go to the next best stop
  if (wait_state == WAIT_STATE_ON && millis() - wait_time > WAIT_TIME) {
    wait_state = WAIT_STATE_OFF;
    if (stops_length > 0) {
      qsort(stops, stops_length, sizeof(Stop *), stops_compare);
      goto_first_stop();
    }
  }

  // Check off the lift state is waiting
  digitalWrite(LED_PIN, lift_state == LIFT_STATE_WAITING);

  // If lift state is waiting get pressed key check if it's a good key and add stop
  if (lift_state == LIFT_STATE_WAITING) {
    uint8_t etage = keypad.getKey() - '0';
    if (etage >= 1 && etage <= LIFT_ETAGES_COUNT && etage != lift_etage) {
      update_stops(etage, etage > lift_etage ? UP : DOWN, true);
      goto_first_stop();
    }
  }

  // Print the information that the master is going to send to all the slaves
  #ifdef DEBUG
    Serial.print("<- lift_etage = ");
    Serial.println(lift_etage);
    Serial.print("<- lift_state = ");
    Serial.println(lift_state);
  #endif

  // Loop over all the etages and send and receive information
  for (uint8_t etage = 1; etage <= LIFT_ETAGES_COUNT; etage++) {
    // Send the information conform protocol
    uint8_t lift_stop_accepted = 0;
    for (uint8_t i = 0; i < stops_length; i++) {
      if (stops[i]->etage == etage && !stops[i]->reported) {
        lift_stop_accepted = stops[i]->direction;
        stops[i]->reported = 1;
        break;
      }
    }

    Wire.beginTransmission(etage);
    Wire.write(lift_etage);
    Wire.write(lift_state);
    Wire.write(lift_stop_accepted);
    Wire.endTransmission();

    // Request other information conform protocol
    if (Wire.requestFrom(etage, PROTOCOL_REQUEST_MESSAGE_LENGTH) == PROTOCOL_REQUEST_MESSAGE_LENGTH) {
      // Read ping value and ignore
      uint8_t ping = Wire.read();
      #ifdef DEBUG
        Serial.print("-> ping[");
        Serial.print(etage);
        Serial.print("] = ");
        Serial.println(ping);
      #endif

      // Read lift is here value
      uint8_t lift_is_here = Wire.read();
      #ifdef DEBUG
        Serial.print("-> lift_is_here[");
        Serial.print(etage);
        Serial.print("] = ");
        Serial.println(lift_is_here);
      #endif

      // If lift is here then update lift etage
      if (lift_is_here == 1) {
        lift_etage = etage;
      }

      // Read request lift stop value
      int8_t lift_request_stop = Wire.read();
      #ifdef DEBUG
        Serial.print("-> lift_request_stop[");
        Serial.print(etage);
        Serial.print("] = ");
        Serial.println(lift_request_stop);
      #endif

      // Handle request stop
      if (lift_request_stop != 0) {
          update_stops(etage, lift_request_stop, 0);
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

  #ifdef DEBUG
    delay(500);
  #endif
}
