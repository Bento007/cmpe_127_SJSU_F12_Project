/**
 * @file light_sensor.hpp
 */
#ifndef LIGHT_SENSOR_HPP_
#define LIGHT_SENSOR_HPP_



#include "src/io_device.hpp"  // Base class



/**
 * Light Sensor class used to get light reading from the Board's Light Sensor
 *
 * @ingroup BoardIO
 */
class Light_Sensor : public IO_Device, public SingletonTemplate<Light_Sensor>
{
    public:
        bool init(); ///< Initializes this device, @returns true if successful

        unsigned short getLightReading(); ///< @returns light sensor reading
        const char* getValueAsString();   ///< @returns light sensor reading as a string

    private:
        Light_Sensor() { }  ///< Private constructor of this Singleton class
        friend class SingletonTemplate<Light_Sensor>;  ///< Friend class used for Singleton Template

        /// The ADC pin the sensor's analog output is attached to
        static const char mAdcChannelOfSensor = 2;
};




#endif /* LIGHT_SENSOR_HPP_ */
