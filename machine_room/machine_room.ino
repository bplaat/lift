
//https: //www.arduino.cc/en/Tutorial/MasterWriter

#include <Wire.h>
#include "addresses.h"

//this function makes a link between the serial bus and the I2C.
//be aware that line endings are characters too
void sent_recieve()
{
    int bytes_to_recieve = Serial.available();
    if (bytes_to_recieve > 0)
    {
        Wire.beginTransmission(JACO_ETAGE);
        for (uint16_t i = 0; i < bytes_to_recieve; i++)
        {
            char recieved_byte = Serial.read();
            Wire.write(recieved_byte); // sends send recieved byte
            Serial.print(recieved_byte);
        }
        Wire.endTransmission(); // stop transmitting
    }
}
void setup()
{
    Wire.begin(); // join i2c bus (address optional for master)
    Serial.begin(9600);
    Serial.println("MACHINE_ROOM is online");
}

void loop()
{
    sent_recieve();
}