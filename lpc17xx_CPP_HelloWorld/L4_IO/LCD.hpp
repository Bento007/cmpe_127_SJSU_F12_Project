/*
 * LCD.hpp
 *
  *  Created on: Oct 13, 2012
 *      Author: Trent Smith
 *
 *  Use: to display strings on the LCD screen
 *  through a SPI connection'
 *
 *  @ingroup BoardIO
 */

#ifndef LCD_HPP_
#define LCD_HPP_

#include "src/io_device.hpp"  // Base class

class LCD :public IO_Device, public SingletonTemplate<LCD>
{
    public:
        bool init();    // Initializes this device, @returns true if successful
        void setup(unsigned int brightness, unsigned int contrast, bool on);   //default setup
        void on();  //turns on screen;
        void off(); //turns off screen;
        void clear();   //clears the screen;
        void clear(unsigned int row);    // clears a row
        void B_Light(unsigned int level);  //changes backlight brightness
        void Contrast(unsigned int level); //changes screen contrast
        void write(const char* words, unsigned int row); //writes to a specific row.
        void write(const char* words);//writes starting from home position

    private:
        LCD() {}   ///< Private constructor of this Singleton class
        friend class SingletonTemplate<LCD>;  ///< Friend class used for Singleton Template
};
#endif /* LCD_HPP_ */
