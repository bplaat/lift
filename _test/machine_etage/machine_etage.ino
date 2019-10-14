#include <Wire.h>

#define DEBUG

#define ETAGE_ADDRESSES_LENGTH 2
#define PROTOCOL_RETURN_MESSAGE_LENGTH 3

uint8_t lift_cabine_etage = 0;
int8_t lift_cabine_direction = 0;

void setup() {
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println("Machine Etage");
  #endif
  Wire.begin();
}

void loop() {
  #ifdef DEBUG
    Serial.print("<- lift_cabine_etage = ");
    Serial.println(lift_cabine_etage);
    Serial.print("<- lift_cabine_direction = ");
    Serial.println(lift_cabine_direction);
  #endif

  for (uint8_t address = 1; address <= ETAGE_ADDRESSES_LENGTH; address++) {
    Wire.beginTransmission(address);
    Wire.write(lift_cabine_etage);
    Wire.write(lift_cabine_direction);
    Wire.write(0);
    Wire.endTransmission();

    Wire.requestFrom(address, PROTOCOL_RETURN_MESSAGE_LENGTH);
    if (Wire.available()) {
      uint8_t ping = Wire.read();
      #ifdef DEBUG
        Serial.print("-> ping[");
        Serial.print(address);
        Serial.print("] = ");
        Serial.println(ping);
      #endif

      if (ping != 1) {
        // stop working?
      }

      uint8_t lift_cabine_is_here = Wire.read();
      #ifdef DEBUG
        Serial.print("-> lift_cabine_is_here[");
        Serial.print(address);
        Serial.print("] = ");
        Serial.println(lift_cabine_is_here);
      #endif

      if (lift_cabine_is_here == 1) {
        lift_cabine_etage = address;
      }

      int8_t lift_request_stop = Wire.read();
      #ifdef DEBUG
        Serial.print("-> lift_request_stop[");
        Serial.print(address);
        Serial.print("] = ");
        Serial.println(lift_request_stop);
      #endif

      if (lift_request_stop == -1) {

      }

      if (lift_request_stop == 1) {

      }
    }
  }

  #ifdef DEBUG
    delay(500);
  #endif
}
