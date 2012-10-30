/*
 * KEYPAD.h
 *
 *  Created on: Sep 21, 2012
 *      Author: Trent Smith
 *
 *  Use: For interfacing with a 4x4 matrix keypad.
 *  Keypad class used to get key value of the key
 *  being pressed.
 *  update 10/12/12
 *      -added getChar, getVal using pointers and through return function.
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "src/io_device.hpp"  // Base class

/**
 * Keypad class used to get key value of the key being pressed.
 *
 * @ingroup BoardIO
 */
class Keypad :public IO_Device, public SingletonTemplate<Keypad>
{
    public:
        bool init(); ///< Initializes this device, @returns true if successful
        void setCol(int col);//sets the column to high and the rest to low
        bool chkRow(int row);//checks rows individually
        int chkRow();//checks all the rows
        int getVal();//returns the buttons number
        bool getVal(int*);//returns an integer via pointer
        char getChar();//returns the char associated to button CHECKED
        bool getChar(char*);//returns the char associated to button via pointer. CHECKED
        //const char* getValueAsString( int x);


    private:
        Keypad() {}   ///< Private constructor of this Singleton class
        friend class SingletonTemplate<Keypad>;  ///< Friend class used for Singleton Template
};

#endif /* KEYPAD_H_ */
