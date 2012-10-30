/**
 * @file IR_sensor.hpp
 */
#ifndef IR_SENSOR_HPP_
#define IR_SENSOR_HPP_



#include "src/io_device.hpp"  // Base class



/**
 * IR Sensor class used to get signals from the on-board IR Sensor
 * This sensor can decode a remote's IR signals, such as a TV remote control.
 *
 * To make sure no decoded signals are over-written, or you do not get a
 * decoded signal that is very obsolete, the user should periodically
 * check isIRCodeReceived() every 50ms, and buffer signals externally.
 *
 * If a signal is decoded, getLastIRCode() will provide it, and clear out
 * the buffer to receive future signal.  If getLastIRCode() is not called
 * regularly, it is possible that this may return an older signal, even if
 * it was many hours ago.
 *
 * @ingroup BoardIO
 */
class IR_Sensor : public IO_Device, public SingletonTemplate<IR_Sensor>
{
    public:
        bool init(); ///< Initializes this device, @returns true if successful

        /// @returns true if an IR signal was decoded and is available to read
        bool isIRCodeReceived();

        /// @returns The last IR signal that was decoded or 0 if nothing was decoded
        unsigned int getLastIRCode();

        /// If an IR signal was decoded, this @returns the signal as a string
        const char* getValueAsString();

    private:
        /// Private constructor of this Singleton class
        IR_Sensor() {}
        friend class SingletonTemplate<IR_Sensor>;  ///< Friend class used for Singleton Template
};

#endif /* IR_SENSOR_HPP_ */
