#ifndef __F3D_H__
#define __F3D_H__

#include <C3DFBS.h>

typedef enum
{
    COMMS_I2C = 0,
    COMMS_SPI = 1,
} F3DMode;

typedef struct
{
    float x;
    float y;
    float z;
} F3DVector;

class F3D
{
private:
    C3DFBS *_sensor;
    F3DMode _comms_mode;
    uint8_t _buffer[3 * sizeof(float)]; // Stores x,y,z force data
    C3DFBS::status_t _status;
    bool _streaming = false;
    uint32_t _dataFields;

    /**
     * Start the data stream, configure the sensor, and begin data streaming.
     *
     * @param error pointer to hold any error that occurs during execution
     */
    void start(int *error = nullptr);

    /**
     * Stop the data stream
     */
    void stop(int *error = nullptr);

    /**
     * A helper function to set error code
     */
    void setError(int *error, C3DFBS::status_t status);

public:
    /**
     * F3D class constructor.
     *
     * @param mode The communication mode to be used (COMMS_I2C/COMMS_SPI).
     * @param inputArg If mode = COMMS_I2C, inputArg is the i2c address of the C3DFBS device. If mode = COMMS_SPI, inputArg is the chip select pin for the C3DFBS device.
     * @param interrupt The interrupt pin for this device
     * @param nReset The reset pin for this device (active LOW)
     */
    F3D(F3DMode mode, uint8_t inputArg, int interrupt, int nReset);

    /**
     * F3D class destructor.
     */
    virtual ~F3D();

    /**
     * Initialise the F3D sensor.
     */
    void begin(void);

    /**
     * Read force data from the F3D sensor.
     *
     * @param error pointer to hold any error that occurs during execution
     * @return 3D vector representing the forces read from the sensor
     */
    F3DVector read(int *error = nullptr);

    /**
     * Reset the F3D sensor.
     *
     * @param error pointer to hold any error that occurs during execution
     */
    void reset(int *error = nullptr);

    /**
     * Zero the F3D sensor (i.e. remove sensor bias). Call this function when no forces are being applied.
     *
     * @param error pointer to hold any error that occurs during execution
     */
    void zero(int *error = nullptr);

    /**
     * Retrieves the firmware version from the F3D sensor.
     *
     * @param error pointer to hold any error that occurs during execution
     * @return version information as a String
     *
     */
    String version(int *error = nullptr);

    /**
     * Retrieve the last error that occurred during communcation with the F3D sensor.
     *
     */
    int lastError(void);

    /**
     * Print timestamp and 3D force data to the specified destination.
     *
     * @param forces The 3D force vector to be printed.
     * @param binary Flag indicating whether to print in binary format (default: false, print as formatted ASCII).
     * @param dest The destination to print the data to.
     */
    static void print(F3DVector forces, bool binary = false, Print *dest = &Serial);
};

#endif