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
    f3d.begin();
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
}
