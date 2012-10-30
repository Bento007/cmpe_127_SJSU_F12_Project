/**
 * @file LED.hpp
 */
#ifndef LED_HPP__
#define LED_HPP__



#include "src/io_device.hpp"  // Base class



/**
 * LED class used to control the Board's 8 output LEDs
 *
 * @ingroup BoardIO
 */
class LED : public IO_Device, public SingletonTemplate<LED>
{
    public:
        bool init(); ///< Initializes this device, @returns true if successful

        void on(int ledNum);    ///< Turns  ON LED. @param ledNum The LED # from 1-8
        void off(int ledNum);   ///< Turns OFF LED. @param ledNum The LED # from 1-8
        void setAll(char value);///< Sets 8-bit value of 8 LEDs; 1 bit per LED

        /// @returns the 8-LED state.  If LED2 is on, will return "00000010"
        const char* getValueAsString();

    private:
        unsigned char mLedValue; ///< Current bits set on the LEDs
        static const unsigned int mLedPinsBitMask = 0xC713;

        LED() {}    ///< Private constructor of this Singleton class
        friend class SingletonTemplate<LED>;  ///< Friend class used for Singleton Template
};

#endif /* LED_HPP__ */
