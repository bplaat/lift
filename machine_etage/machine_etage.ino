// ############################
// #### Machine Lift Etage ####
// ############################

// Include the wire library for I2C
#include <Wire.h>

// Enable the DEBUG flag
#define DEBUG

// Global lift variables
#define LIFT_ETAGES_COUNT 5
#define PROTOCOL_RETURN_MESSAGE_LENGTH 3

#define UP 1
#define DOWN -1

#define LIFT_STATE_STILL 0
#define LIFT_STATE_MOVING 1
#define LIFT_STATE_WAITING 2

uint8_t lift_etage = 0;
uint8_t lift_state = LIFT_STATE_STILL;

// Stops code

// The stop struct
typedef struct Stop {
  uint8_t etage;
  int8_t direction;
  uint8_t destination;
  uint8_t reported;
} Stop;

Stop *stop_new(uint8_t etage, int8_t direction, uint8_t destination, uint8_t reported) {
  Stop *stop = (Stop *)malloc(sizeof(Stop));
  stop->etage = etage;
  stop->direction = direction;
  stop->destination = destination;
  stop->reported = reported;
  return stop;
}

void stop_free(Stop *stop) {
  free(stop);
}

// Main code

void setup() {
  // When in debug mode is enabled init the serial communication
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println("Machine Etage");
  #endif

  // Init the I2C functions
  Wire.begin();
}

void loop() {
  // Print the information that the master is going to send to all the slaves
  #ifdef DEBUG
    Serial.print("<- lift_etage = ");
    Serial.println(lift_etage);
    Serial.print("<- lift_state = ");
    Serial.println(lift_state);
  #endif

  // Loop over all the etages and send and receive information
  for (uint8_t address = 1; address <= LIFT_ETAGES_COUNT; address++) {
    // Send the information conform protocol
    Wire.beginTransmission(address);
    Wire.write(lift_etage);
    Wire.write(lift_state);
    Wire.write(0);
    Wire.endTransmission();

    // Request other information conform protocol
    if (Wire.requestFrom(address, PROTOCOL_RETURN_MESSAGE_LENGTH) == PROTOCOL_RETURN_MESSAGE_LENGTH) {
      // Read ping value and ignore
      uint8_t ping = Wire.read();
      #ifdef DEBUG
        Serial.print("-> ping[");
        Serial.print(address);
        Serial.print("] = ");
        Serial.println(ping);
      #endif

      // Read lift is here value
      uint8_t lift_is_here = Wire.read();
      #ifdef DEBUG
        Serial.print("-> lift_is_here[");
        Serial.print(address);
        Serial.print("] = ");
        Serial.println(lift_is_here);
      #endif

      // If lift is here then update lift etage
      if (lift_is_here == 1) {
        lift_etage = address;
      }

      // Read request lift stop value
      int8_t lift_request_stop = Wire.read();
      #ifdef DEBUG
        Serial.print("-> lift_request_stop[");
        Serial.print(address);
        Serial.print("] = ");
        Serial.println(lift_request_stop);
      #endif

      // Handle request stop
      if (lift_request_stop == UP || lift_request_stop == DOWN) {

      }
    }

    // If slave etage didn't respond with the right amount of bytes then this exception will handle it
    #ifdef DEBUG
      else {
        Serial.print("Etage ");
        Serial.print(address);
        Serial.println(" didn't respond the right amount of bytes!");
      }
    #endif
  }

  #ifdef DEBUG
    delay(500);
  #endif
}
