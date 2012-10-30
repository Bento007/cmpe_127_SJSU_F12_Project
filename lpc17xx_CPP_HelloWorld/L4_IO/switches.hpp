/**
 * @file switches.hpp
 */
#ifndef SWITCHES_HPP__
#define SWITCHES_HPP__



#include "src/io_device.hpp"  // Base class



/**
 * Switches class used to get switch values from on-board switches
 *
 * @ingroup BoardIO
 */
class Switches : public IO_Device, public SingletonTemplate<Switches>
{
    public:
        bool init(); ///< Initializes this device, @returns true if successful

        /// @returns The 8-bit char containing bit values of the 8 switches, 1 bit per switch
        unsigned char getSwitchValues();

        int getSwitchValues(int);//gets the number of the switch hit

        /**
         * @param num   The switch number to test from 1-8
         * @returns true if the switch is active, or false otherwise
         */
        bool getSwitch(int num);

        /// @returns binary string of switches, such as "1100.0011"
        const char* getValueAsString();

    private:
        Switches() {}   ///< Private constructor of this Singleton class
        friend class SingletonTemplate<Switches>;  ///< Friend class used for Singleton Template
};

#endif /* SWITCHES_HPP__ */
