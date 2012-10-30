/**
 * @file main.cpp
 * @brief This is the application entry point.
 *          FreeRTOS and stdio printf is pre-configured to use uart0_min.h before main() enters.
 *          You can over-ride input/output UART functions to use alternate interrupt-driven UART0 driver.
 *          @see L0_LowLevel/io_functions.h if you wish to override printf/scanf functions
 */
#include <stdio.h>
#include "io.hpp"
#include "utilities.h"
#include "sysConfig.h"
#include "str.hpp"
#define SAVE 10  //number of characters from keypad to save

int main(void)
{
/////////////////////////////////definition of characters
   int i; //walker
   char keys[SAVE]="---------";  // used for setting the left and right 7-segment Display initially
   bool header = true;
   str current;
   delay_ms( 200 );     //delay for LCD to initialize

   while(1)   //Infinite loop to run program
   {

       //LCD temperature row 2
       current = TS.getValueAsString();//saves current temperature
       LC.write(current(),1);//updates the temperature in row 2
       printf("%s\n",current());//prints temperature to stdio

       if(SW.init() && SW.getSwitchValues() !=0 )//switch presses
       {
           //row 3 buttons
           current=SW.getValueAsString();//get the current switch values
           LC.init();//prepares register to write to LCD 
           LC.write(current(),2);//writes to rwo 3
           printf("%s\n",SW.getValueAsString());
           for(i=1; i<=8; i++)
           {
               if(SW.getSwitch(i))//upadtes 7-seg
               {
                   LD.setNumber((char)i);
                   LE.on(i);
               }
               else
               {
                   LE.off(i);
               }
           }
       }
       /*if all the registers are set to read from keypad then continue.
        * This statement will read a button from keypad and display it
        * on Stdio and and the 7-seg display.
        */
		//setup row 4 keypad
       KP.init();
       if(KP.getChar(keys))      //if buttons pressed save value in right
       {
           printf("KP: %s\n",keys);   //prints to stdio
           LD.setRightDigit(keys[0]);//set right LED to 'right'
           LD.setLeftDigit(keys[1]);  //set left LED to 'left'
           //LCD row 4 keypad
           current = "KP: "; current +=  keys;
           LC.init();//prepares register to write to LCD
           LC.write(current(),3);//updates row 4
           //saves previous key presses.
           for(i = SAVE-2; i>0; i--)
           {
               keys[i]=keys[i-1];
           }
       }

       //setup & LCD row 1
       if((SW.getSwitch(8) && SW.getSwitch(1)) || header)
       {
           current = "Trent B. Smith";
           LC.init();//prepares register to write to LCD
           LC.setup(2,40,true); //sets LCD settings
           LC.write(current(),0);//write name to rown1
           header= false;//prevents repeating
       }
   }
   return -1;
}
