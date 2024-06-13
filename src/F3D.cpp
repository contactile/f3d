#include <F3D.h>
#include <assert.h>

F3D::F3D(F3DMode mode, uint8_t inputArg, int interrupt, int nReset)
{
    _comms_mode = mode;

    if (_comms_mode == COMMS_SPI)
    {
        _sensor = new C3DFBS(C3DFBS::COMMS_SPI, inputArg, interrupt, nReset);
    }
    else //(mode == COMMS_I2C), default
    {
        _i2c_address = inputArg;   
        _sensor = new C3DFBS(C3DFBS::COMMS_I2C, _i2c_address, interrupt, nReset);
    }
}

F3D::~F3D()
{
    delete _sensor;
}

void F3D::begin()
{
    if(_comms_mode == COMMS_I2C)
    {
        // Check that the address is valid
        assert("I2C address out of valid range (0x50-0x72)" && _i2c_address > 0x4F && _i2c_address < 0x73);   
        assert("I2C address 0x55 is reserved" && _i2c_address != 0x55);   
    } 

    // Place the sensor into the selected comms mode via bootstrapping
    _sensor->setCommunicationProtocol(_comms_mode);
    // Initialise the sensor
    _sensor->begin();
}

bool F3D::connect()
{
    // Set the I2C address if in I2C mode
    if(_comms_mode == COMMS_I2C)
    {
        // Try to locate the sensor    
        if(ping(_i2c_address)) // Try the address specified in the constructor
        {
            // Found a device with the expected address. Fall-thru to verify connection
        }    
        else if(ping(DEFAULT_I2C_ADDRESS)) // Try the default address
        {
            _sensor->changeI2CAddress(_i2c_address, DEFAULT_I2C_ADDRESS);
        }
        else
        {   // Nothing responded to the default address.

            // Reset the sensor...
            _sensor->reset();

            // Now try to locate a 3DFBS device on another address.
            // Scan the valid range of addresses for C3DFBS sensors to see if any respond.
            for(int current_address=0x50; current_address<0x72; current_address++)
            {
                if(current_address == 0x55)
                    continue; // 0x55 is a reserved address

                if(ping(current_address)) 
                {   // Found device on the I2C bus.
                    
                    // Now change its address to that specified in the constructor
                    auto status = _sensor->changeI2CAddress(_i2c_address, current_address);

                    if(status == C3DFBS::SUCCESS)
                        break; // Found a device and successfully changed its address
                }                
            }
        }
    }

    // Verify connection
    return _sensor->isAlive();
}

bool F3D::ping(int address)
{
    Wire.beginTransmission(address);
    return (Wire.endTransmission() == 0);
}

void F3D::start(int *error)
{
    /* Stop data stream first */
    stop(error);

    /* Configure the sensor for 1kHz*/
    _status = _sensor->setDataFrequency(1000);
    if (_status != C3DFBS::SUCCESS)
    {
        setError(error, _status);
        return;
    }

    _dataFields = C3DFBS::AllForce;
    _status = _sensor->setDataFields(_dataFields);
    if (_status != C3DFBS::SUCCESS)
    {
        setError(error, _status);
        return;
    }

    // Start data streaming
    _status = _sensor->startDataStream();
    if (_status != C3DFBS::SUCCESS)
    {
        setError(error, _status);
        return;
    }

    _streaming = true;
}

F3DVector F3D::read(int *error)
{
    F3DVector forces = {NAN, NAN, NAN};

    if (!_streaming)
        start(error);

    /* Read data stream */
    _status = _sensor->readDataStream(_buffer);
    if (_status != C3DFBS::SUCCESS)
    {
        setError(error, _status);
        return forces;
    }

    // Override default NAN values if data is included
    if (_dataFields & C3DFBS::ForceZ)
        forces.z = ((float)_buffer[0]);

    if (_dataFields & C3DFBS::ForceY)
        forces.y = ((float)_buffer[1]);

    if (_dataFields & C3DFBS::ForceX)
        forces.x = ((float)_buffer[2]);

    return forces;
}

void F3D::print(F3DVector forces, bool binary, Print *dest)
{
    uint32_t timestamp = millis();

    if (binary)
    {
        dest->write((uint8_t *)&timestamp, sizeof(uint32_t));
        dest->write((uint8_t *)&forces, sizeof(F3DVector));
    }
    else
    { // Print as formatted ASCII
        dest->printf("Timestamp: %d, ", timestamp);
        dest->printf("Fx: %3.2f, ", forces.x);
        dest->printf("Fy: %3.2f, ", forces.y);
        dest->printf("Fz: %3.2f", forces.z);

        if (isnan(forces.x) || isnan(forces.y) || isnan(forces.z))
            dest->print(" (unbiased)");

        dest->print("\n");
    }
}

void F3D::stop(int *error)
{
    _status = _sensor->stopDataStream();
    _streaming = false;

    if (_status != C3DFBS::SUCCESS)
    {
        setError(error, _status);
        return;
    }
}

void F3D::reset(int *error)
{
    bool was_streaming = _streaming;

    _sensor->reset();
    _streaming = false;

    if (_status != C3DFBS::SUCCESS)
    {
        setError(error, _status);
        return;
    }    
    
    // Try to start stream again
    if (was_streaming)
        start(error);

    if (_status != C3DFBS::SUCCESS)
    {
        setError(error, _status);
        return;
    }
}

void F3D::zero(int *error)
{
    bool was_streaming = _streaming;
    if (_streaming)
        stop(error);

    _status = _sensor->removeBias();

    // Try to start stream again
    if (was_streaming)
        start(error);

    if (_status != C3DFBS::SUCCESS)
    {
        setError(error, _status);
        return;
    }
}

String F3D::version(int *error)
{
    String ver;

    bool was_streaming = _streaming;
    if (_streaming)
        stop(error);

    _status = _sensor->getVersion(&ver);

    // Try to start stream again
    if (was_streaming)
        start(error);

    if (_status != C3DFBS::SUCCESS)
    {
        setError(error, _status);
    }

    return ver;
}

int F3D::lastError(void)
{
    return (int)_status;
}

void F3D::setError(int *error, C3DFBS::status_t status)
{
    if (error != nullptr)
        *error = (int)status;
}
