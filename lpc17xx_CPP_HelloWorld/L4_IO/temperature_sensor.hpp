/**
 * @file temperature_sensor.hpp
 */
#ifndef TEMPERATURE_SENSOR_HPP_
#define TEMPERATURE_SENSOR_HPP_



#include "src/io_device.hpp"        // Base class
#include "src/I2C_Device_Base.hpp"  // I2C Device base class


/**
 * Temperature Sensor class used to get temperature reading from the on-board sensor
 *
 * @ingroup BoardIO
 */
class TemperatureSensor : public IO_Device,  public I2C_Device_Base, public SingletonTemplate<TemperatureSensor>
{
    public:
        bool init(); ///< Initializes this device, @returns true if successful

        float getCelsius();   ///< @returns floating-point reading of temperature in Celsius
        float getFarenheit(); ///< @returns floating-point reading of temperature in Farenheit

        /// @returns string pointer containing the temperature reading in celsius with degree symbol
        const char* getValueAsString();

    private:
        /// Private constructor of this Singleton class
        TemperatureSensor() :  I2C_Device_Base(Address_TemperatureSensor)
        {
        }

        friend class SingletonTemplate<TemperatureSensor>;  ///< Friend class used for Singleton Template
};


#endif /* TEMPERATURE_SENSOR_HPP_ */
