/**
 * @file io_device.hpp
 * @brief This class provides an IO device base class and mandates
 *          specific API to be available, such as init()
 */

#ifndef IO_DEVICE_HPP__
#define IO_DEVICE_HPP__

#include "str.hpp"
#include "singletonTemplate.hpp"

/**
 * IO Device base class that mandates minimum functionality of
 * an IO device such as a light sensor.
 *
 * @ingroup BoardIO
 */
class IO_Device
{
    public:
        virtual bool init()=0;   ///< Parent class must have init() method

        /**
         * Generic method to get the I/O device output as a string
         * @returns     The pointer to the string containing IO device value
         */
        virtual const char* getValueAsString()
        {
            mStr = "Error: Function not implemented";
            return mStr();
        }

        /**
         * This is the () operator to do the lazy-man's way of doing getValueAsString()
         * Example: Instead of IR.getValueAsString(), we can simply do: IR()
         */
        const char* operator()() { return getValueAsString(); }

    protected:
        virtual ~IO_Device()   { }  ///< Virtual destructor of this class
        IO_Device() : mStr(32) { }  ///< Protected constructor to disallow this class from being created

        str mStr; ///< str  used by getValueAsString()
};



/**
 * I2C Addresses for on-board devices attached to I2C1 Bus
 */
enum Board_I2C_Device_Addresses{
    Address_AccelerationSensor = 0x38,
    Address_TemperatureSensor  = 0x90,
    Address_LED_Display = 0x40
};



#endif /* IO_DEVICE_HPP__ */
