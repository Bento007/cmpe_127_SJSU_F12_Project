/**
 * @file LED_Display.hpp
 */
#ifndef LED_DISPLAY_HPP__
#define LED_DISPLAY_HPP__



#include "src/io_device.hpp"        // Base class
#include "src/I2C_Device_Base.hpp"  // I2C Device Base class


/**
 * LED Display class to manipulate the on-board 2 digit LED display
 *
 * @ingroup BoardIO
 */
class LED_Display : public IO_Device, public I2C_Device_Base, public SingletonTemplate<LED_Display>
{
    public:
        bool init(); ///< Initializes this device, @returns true if successful

        /**
         * Sets the number on LED display
         * @param num   A number less than 100
         */
        void setNumber(char num);

        /**
         * @{ Single Digit Manipulation Functions
         * Sets the left and right alpha-numeric display individually
         * @param alpha The alpha to set, such as "B" or "9"
         * @note  Note all characters can be displayed on this 7-segment display
         */
        void setLeftDigit(char alpha);
        void setRightDigit(char alpha);
        /** @} */

        /// @returns the characters currently displayed on this LED Display
        const char* getValueAsString();

    private:
        char mNumAtDisplay[2]; ///< The number currently being displayed

        /// Private constructor of this Singleton class
        LED_Display() : I2C_Device_Base(Address_LED_Display)
        {
        }
        friend class SingletonTemplate<LED_Display>;  ///< Friend class used for Singleton Template

        /// Enumeration of the register map
        typedef enum {
            inputPort0, inputPort1,
            outputPort0, outputPort1,
            polarityPort0, polarityPort1,
            cfgPort0, cfgPort1
        } __attribute__ ((packed)) RegisterMap;
};




#endif /* LED_DISPLAY_HPP__ */
