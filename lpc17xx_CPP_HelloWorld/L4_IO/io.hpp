/**
 *  @file io.hpp
 *  @brief This file provides access to all IO devices of the board
 *  @ingroup BoardIO
 *
 * Version: 06192012    Initial
 */
#ifndef IO_SHORTCUTS_HPP_
#define IO_SHORTCUTS_HPP_



#include "acceleration_sensor.hpp"
#include "IR_sensor.hpp"
#include "LED_Display.hpp"
#include "LED.hpp"
#include "light_sensor.hpp"
#include "switches.hpp"
#include "temperature_sensor.hpp"
#include "KEYPAD.hpp"
#include "LCD.hpp"


/**
 * @{ Short-hand notation of getting to the on-board IO Devices
 * Example:  puts(A.getValueAsString());
 */
#define AS      Acceleration_Sensor::getInstance()  ///< Acceleration Sensor
#define IS      IR_Sensor::getInstance()            ///< IR input sensor
#define LS      Light_Sensor::getInstance()         ///< Light Sensor
#define TS      TemperatureSensor::getInstance()    ///< Temperature Sensor
#define LD      LED_Display::getInstance()          ///< 2-Digit LED Display
#define LE      LED::getInstance()                  ///< 8 On-board LEDs
#define SW      Switches::getInstance()             ///< 8 On-board Switches
#define KP      Keypad::getInstance()               ///< 4x4 matrix keypad
#define LC     LCD::getInstance()                  ///< 4x20 LCD screen
/** @} */



#endif /* IO_SHORTCUTS_HPP_ */
