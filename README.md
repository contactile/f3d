# F3D

<a href="https://contactile.com/"><img src="https://github.com/contactile/c3dfbs/assets/122410361/c9684832-b24c-4592-ae98-df30c9543855" alt="Contactile logo"></a>

<h3 align="center" style="font-size:40px">3DFBS Host Library</h3>

<div align="center">

![GitHub Release](https://img.shields.io/github/v/release/contactile/f3d)
![GitHub Issues or Pull Requests](https://img.shields.io/github/issues/contactile/f3d)
![GitHub License](https://img.shields.io/github/license/contactile/f3d)

</div>

<div align="center">

A high-level wrapper on the [C3DFBS class](https://github.com/contactile/c3dfbs), for reading data from [Contactile](https://contactile.com/) 3DFBS sensors.

</div>

## Installation

The **F3D** class includes the [C3DFBS class](https://github.com/contactile/c3dfbs), so two dependencies must be added to your project.

### Platform IO

Include the F3D library in your code by adding these libraries to your _platformio.ini_ file.

```
lib_deps =
   https://github.com/contactile/c3dfbs.git @ ^0.3.0
   https://github.com/contactile/f3d.git @ ^1.0.0
```

### Arduino

The easiest way to include the F3D library in an Arduino sketch is to first download the [F3D repository](https://github.com/contactile/f3d/archive/refs/heads/main.zip), and the [C3DFBS repository](https://github.com/contactile/c3dfbs/archive/refs/heads/main.zip). Then install both libraries in the Arduino IDE by navigating to _Sketch > Include Library > Add .ZIP Library..._ and selecting the zip files.

## Usage

This is a complete working example. This library was designed to have a simple interface; for advanced functionality, use the [C3DFBS class](https://github.com/contactile/c3dfbs).

```cpp
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
```

## License

The F3D host library is licensed under the [GNU General Public License v3.0](https://github.com/contactile/f3d/blob/main/LICENSE).
