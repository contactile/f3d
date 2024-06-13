#include <Arduino.h>
#include <F3D.h>

int error;
F3DVector forces;
const uint8_t int_pin = 8;   // Interrupt pin
const uint8_t reset_pin = 9; // Reset pin

// Run sensor in SPI Mode
const uint8_t ss_pin = SS; // Slave select pin
F3D f3d = F3D(COMMS_SPI, ss_pin, int_pin, reset_pin);

// // Run sensor in I2C Mode
// uint8_t i2c_address = 0x57;
// F3D f3d = F3D(COMMS_I2C, i2c_address, int_pin, reset_pin);

void setup()
{
    Serial.begin(115200);

    // Wait for serial connection
    while(!Serial);
    f3d.begin();

    // Establish a connection with the sensor
    Serial.println("Establishing a connection...");
    while(!f3d.connect())
    {
        Serial.println(" Error: F3D sensor missing - check connections.");
        delay(1000);
    }

    String ver = f3d.version(&error);
    Serial.printf(" OK. Connected to F3D sensor, version %s\n", ver.c_str());

    f3d.zero();
}

void loop()
{
    // Read 3D forces at up to 1000Hz
    forces = f3d.read(&error);

    if (error)
    {
        // Print error
        Serial.printf("Read failed with error code %d\n", error);
        delay(1000);
    }
    else
    {
        // Print force data
        F3D::print(forces);

        // Delay to prevent overwhelming the serial port
        delay(20);
    }

    // Receive basic serial commands
    while(Serial.available())
    {
        char c = Serial.read();

        if(c == 'b' || c == 'z')
        {   // bias/zero the sensor
            f3d.zero();
        }
        if(c == 'r')
        {   // reset the sensor
            f3d.reset();
        }
    }
}
