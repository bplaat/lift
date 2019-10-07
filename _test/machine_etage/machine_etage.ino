#include <Wire.h>

#define DEBUG
#define ETAGE_ADDRESSES_LENGTH 2

uint8_t lift_cabine_etage = 0;
uint8_t lift_cabine_direction = 0;

void setup() {
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println("Machine Etage");
  #endif
  Wire.begin();
}

void loop() {
  #ifdef DEBUG
    Serial.print("lift_cabine_etage = ");
    Serial.println(lift_cabine_etage);
    Serial.print("lift_cabine_direction = ");
    Serial.println(lift_cabine_direction);
  #endif
  
  for (uint8_t i = 1; i <= ETAGE_ADDRESSES_LENGTH; i++) {
    Wire.beginTransmission(i);
    Wire.write(lift_cabine_etage);
    Wire.write(lift_cabine_direction);
    Wire.endTransmission();

    Wire.requestFrom(i, 1);
    if (Wire.available()) {
      uint8_t lift_cabine_is_here = Wire.read();

      #ifdef DEBUG
        Serial.print("lift_cabine_is_here[");
        Serial.print(i);
        Serial.print("] = ");
        Serial.println(lift_cabine_is_here);
      #endif
      
      if (lift_cabine_is_here == 1) {
        lift_cabine_etage = i;
      }
    }
  }

  #ifdef DEBUG
    delay(500);
  #endif
}
