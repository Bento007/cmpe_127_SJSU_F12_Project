#include "../io.hpp" // All IO Class definitions
#include "LPC17xx.h"
#include "adc0.h"
#include "utilities.h"
#include "spi1.h"
//#include <stdio.h>  // Debugging
//#include "utilities.h"


/**
 * The following diagram shows bit number corresponding to the LED
 *
 *      0
 *     ---
 *  5 | 6 | 1
 *     ---
 *  4 |   | 2
 *     ---
 *      3   *7
 */
const char LED_DISPLAY_CHARMAP[255] =
{
        0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
        0,    // 32 : Space
        0x86, // 33 : !
        0,0,0,0, 0,0,0,0, 0,0,0, // 34-44
        0x40, // 45 : -
        0x80, // 46 : .
        0x00, // 47 : /
        0xBF, // 48 : 0.
        0xB0, // 49 : 1.
        0xDB, // 50 : 2.
        0xCF, // 51 : 3.
        0xE6, // 52 : 4.
        0xED, // 53 : 5.
        0xFC, // 54 : 6.
        0x87, // 55 : 7.
        0xFF, // 56 : 8.
        0xE7, // 57 : 9.
        0,0,0,0,0,0,0, // 58-64
        0x77, // 65 : A
        0x7F, // 66 : B
        0x39, // 67 : C
        0x3F, // 68 : D
        0x79, // 69 : E
        0x71, // 70 : F
        0x3D, // 71 : E
        // TODO: Need rest of the LED Display char-map
};

bool Acceleration_Sensor::init()
{
    const unsigned char activeModeWith100Hz = (1 << 0) | (3 << 3); // Active Mode @ 100Hz

    writeReg(Ctrl_Reg1, activeModeWith100Hz);
    const char whoAmIReg = readReg(WhoAmI);

    return (mWhoAmIExpectedValue == whoAmIReg);
}
short Acceleration_Sensor::getX()
{
    return (short)get16BitRegister(X_MSB) / 16;
}
short Acceleration_Sensor::getY()
{
    return (short)get16BitRegister(Y_MSB) / 16;
}
short Acceleration_Sensor::getZ()
{
    return (short)get16BitRegister(Z_MSB) / 16;
}
const char* Acceleration_Sensor::getValueAsString()
{
    mStr.printf("%5i:%5i:%5i", getX(), getY(), getZ());
    return mStr();
}

/**
 * The design of the IR Sensor is as follows:
 *  Timer1 captures falling edges of CAP1.0 and timestamps are saved when this happens.
 *  Every time capture happens, a 20ms match interrupt is setup/reset.  When no more
 *  signals arrive, the 20ms match interrupt will decode the timestamps and convert
 *  timestamps into binary signal.
 *
 *  Note that this doesn't decode into real data sent through the IR, but nevertheless,
 *  the decoded signal will be unique per button pressed on an IR remote.
 */
static unsigned int LAST_DECODED_IR_SIGNAL = 0;
#define TIMER1_US_PER_TICK  (100)
extern "C"
{
    void TIMER1_IRQHandler()
    {
        const unsigned int captureMask = (1 << 4);
        const unsigned int MR0Mask     = (1 << 0);
        const unsigned int ticksFor20Ms = (20 * 1000) / TIMER1_US_PER_TICK;

        const  unsigned short maxFallingEdgesPerIRFrame = 32;
        static unsigned short signalCount = 0;
        static unsigned int signalArray[maxFallingEdgesPerIRFrame] = {0};

        // Capture interrupt occurred:
        if(LPC_TIM1->IR & captureMask)
        {
            /**
             * Reload Match Register Interrupt to interrupt 20ms after this point of time
             * If another capture interrupt arrives later, this timer is reset again.
             */
            LPC_TIM1->MR0 = (LPC_TIM1->TC + ticksFor20Ms);

            // Just store the timestamp of this signal
            if(signalCount < maxFallingEdgesPerIRFrame) {
                signalArray[signalCount++] = LPC_TIM1->CR0;
            }

            // Clear the Timer Capture interrupt
            LPC_TIM1->IR = captureMask;
        }
        // Timeout Interrupt to decode the signal
        else if(LPC_TIM1->IR & MR0Mask)
        {
            if(signalCount > 1)
            {
                /**
                 * Time to decode the signals at this timeout
                 * Calculate differences of falling edges
                 */
                for(int i = 0; i < signalCount-1; i++) {
                    signalArray[i] = signalArray[i+1] - signalArray[i];
                }

                /**
                 * First falling edge value should indicate binary 0.
                 * So anything higher than 50% of this value is considered binary 1.
                 */
                const unsigned int binary1Threshold = signalArray[1] + (signalArray[1]/2);
                unsigned int decodedSignal = 0;
                for(unsigned short i=0; i < signalCount-1; i++) {
                    if(signalArray[i] > binary1Threshold) {
                        decodedSignal |= (1 << i);
                    }
                }
                LAST_DECODED_IR_SIGNAL = decodedSignal;
            }

            // Clear the Match Interrupt and signal count
            signalCount = 0;
            LPC_TIM1->IR = MR0Mask;
        }
        else
        {
            // Log error of unexpected interrupt
        }
    }
}

/**
 * IR Sensor is attached to P1.18 - CAP1.0, so it needs TIMER1 to capture the times on P1.18
 */
bool IR_Sensor::init()
{
    // Enable Timer1 that is used to capture CAP1.0 signal
    LPC_SC->PCONP |= (1 << 1);      // Enable Power to Timer1
    LPC_SC->PCLKSEL0 &= ~(3 << 4);  // Clear Clock divider for Timer1
    LPC_SC->PCLKSEL0 |=  (1 << 4);  // Timer1 clock = CPU Clock / 1

    LPC_TIM1->TCR = 1;              // Enable Timer
    LPC_TIM1->CTCR = 0;             // Increment on PCLK
    LPC_TIM1->PR = getCpuClock() / (1000*1000/TIMER1_US_PER_TICK);

    LPC_TIM1->CCR &= ~(7 << 0);            // Clear Bits 2:1:0
    LPC_TIM1->CCR |=  (1 << 2) | (1 << 1); // Enable Falling Edge capture0 with interrupt

    // Enable MR0 interrupt
    LPC_TIM1->MR0 = 0;
    LPC_TIM1->MCR |= (1 << 0);

    // Select P1.18 as CAP1.0 by setting bits 5:4 to 0b11
    LPC_PINCON->PINSEL3 |= (3 << 4);

    // Finally, enable interrupt of Timer1 to interrupt upon falling edge capture
    NVIC_EnableIRQ(TIMER1_IRQn);

    return true;
}
bool IR_Sensor::isIRCodeReceived()
{
    return (0 != LAST_DECODED_IR_SIGNAL);
}
unsigned int IR_Sensor::getLastIRCode()
{
    unsigned int signal = LAST_DECODED_IR_SIGNAL;
    LAST_DECODED_IR_SIGNAL = 0;
    return signal;
}
const char* IR_Sensor::getValueAsString()
{
    mStr.printf("IR: 0x%08X", getLastIRCode());
    return mStr();
}

bool LED_Display::init()
{
    bool devicePresent = checkDeviceResponse();
    if(devicePresent)
    {
        const unsigned char cfgAsOutput = 0x00;
        writeReg(cfgPort0, cfgAsOutput);
        writeReg(cfgPort1, cfgAsOutput);

        setLeftDigit('.');
        setRightDigit('.');
    }

    return devicePresent;
}
void LED_Display::setNumber(char num)
{
    num %= 100;
    setRightDigit(num%10 + '0');

    if(num < 10)
        setLeftDigit(' ');
    else
        setLeftDigit (num/10 + '0');
}
void LED_Display::setLeftDigit(char alpha)
{
    mNumAtDisplay[0] = alpha;
    writeReg(outputPort1, LED_DISPLAY_CHARMAP[(unsigned)alpha]);
}
void LED_Display::setRightDigit(char alpha)
{
    mNumAtDisplay[1] = alpha;
    writeReg(outputPort0, LED_DISPLAY_CHARMAP[(unsigned)alpha]);
}
const char* LED_Display::getValueAsString()
{
    mStr.printf("LED: %c%c", mNumAtDisplay[0], mNumAtDisplay[1]);
    return mStr();
}

/**
 * LED Mapping:
 *      1 : P1.0
 *      2 : P1.1
 *      3 : P1.4
 *      4 : P1.8
 *      5 : P1.9
 *      6 : P1.10
 *      7 : P1.14
 *      8 : P1.15
 */
bool LED::init()
{
    LPC_GPIO1->FIODIR |= mLedPinsBitMask;
    return true;
}
void LED::on(int ledNum)
{
    mLedValue |= (1 << (ledNum-1));
    setAll(mLedValue);
}
void LED::off(int ledNum)
{
    mLedValue &= ~(1 << (ledNum-1));
    setAll(mLedValue);
}
void LED::setAll(char value)
{
    mLedValue = value;
    LPC_GPIO1->FIOCLR = mLedPinsBitMask;

    if(value & (1 << 0))    LPC_GPIO1->FIOSET = (1 << 0);
    if(value & (1 << 1))    LPC_GPIO1->FIOSET = (1 << 1);
    if(value & (1 << 2))    LPC_GPIO1->FIOSET = (1 << 4);
    if(value & (1 << 3))    LPC_GPIO1->FIOSET = (1 << 8);
    if(value & (1 << 4))    LPC_GPIO1->FIOSET = (1 << 9);
    if(value & (1 << 5))    LPC_GPIO1->FIOSET = (1 << 10);
    if(value & (1 << 6))    LPC_GPIO1->FIOSET = (1 << 14);
    if(value & (1 << 7))    LPC_GPIO1->FIOSET = (1 << 15);
}
const char* LED::getValueAsString()
{
    /**
     * Return Binary chars with LED #8 being left-most bit
     */
    mStr = "LEDs: ";
    for(int i=7; i>=0; i--) {
        if(3 == i) mStr += ".";
        mStr += (mLedValue & (1 << i)) ? "1" : "0";
    }
    return mStr();
}

bool Light_Sensor::init()
{
    // Light Sensor is on P0.25, select this as ADC0.2
    LPC_PINCON->PINSEL1 &= ~(3 << 18);
    LPC_PINCON->PINSEL1 |=  (1 << 18);

    adc0_initialize();

    return true;
}
unsigned short Light_Sensor::getLightReading()
{
    return adc0_getReading(mAdcChannelOfSensor);
}
const char* Light_Sensor::getValueAsString()
{
    const int maxAdcValue = 1024;
    mStr.printf("Light: %i%%", (getLightReading() * 100) / maxAdcValue);
    return mStr();
}

/**
 * Switch Mapping:
 * P2.0 : P2.1 : P2.2 : P2.3 : P2.4 : P2.5 : P2.6 : P2.7
 */
bool Switches::init()
{
    LPC_GPIO2->FIODIR0 = 0x00;
    return true;
}
unsigned char Switches::getSwitchValues()
{
    return LPC_GPIO2->FIOPIN0;
}
bool Switches::getSwitch(int num)
{
    return getSwitchValues() & (1 << (num-1));
}
const char* Switches::getValueAsString()
{
    /**
     * Return Binary chars with Switch #8 being left-most bit
     */
    mStr = "SW: ";
    for(int i=8; i>=1; i--) {
        if(4 == i) mStr += ".";
        mStr += getSwitch(i) ? "1" : "0";
    }
    return mStr();
}
//////////////////////////////////////////////////////////////////////////////////
//used to address the correct character.
int KEYPAD_VAL [4][4] = {{1,2,3,10},
                         {4,5,6,11},
                         {7,8,9,12},
                         {14,0,15,13}};
//used to set the appropriate registers to read from the keypad
char KEYPAD_CHAR[4][4] ={{'1','2','3','A'},
                         {'4','5','6','B'},
                         {'7','8','9','C'},
                         {'*','0','#','D'}};
bool Keypad::init()
{
    LPC_GPIO1->FIOMASKH = 0x0;    //unmasks all the need GPIO pins
    LPC_GPIO1->FIODIR2 = 0xFF;    //columns are outputs
    LPC_GPIO1->FIODIR3 = 0x0;     //rows are inputs
    return true;
}
void Keypad::setCol(int col)
{
    /*set the pin for the col to high to
     * check if any buttons in that column
     * are pressed
     */
    switch (col){
        case 1: LPC_GPIO1->FIOPIN2 = 0xF7; break;//set col 1 high
        case 2: LPC_GPIO1->FIOPIN2 = 0xEF; break;//set col 2 high
        case 3: LPC_GPIO1->FIOPIN2 = 0xBF; break;//set col 3 high
        case 4: LPC_GPIO1->FIOPIN2 = 0x7F; break;//set col 4 high
        default: break; //set none
        }

    return;
}
bool Keypad::chkRow(int row)
{
    /*Pre: an interger is passed in
     *Post: If the checked bit(row)
     *is 1 then true will be returned
     */
    switch (row)
    {
        case 1: return !(LPC_GPIO1->FIOPIN3 & (1 << (row-1))); break;//is row 1 high
        case 2: return !(LPC_GPIO1->FIOPIN3 & (1 << (row-1))); break;//is row 2 high
        case 3: return !(LPC_GPIO1->FIOPIN3 & (1 << (row-1))); break;//is row 3 high
        case 4: return !(LPC_GPIO1->FIOPIN3 & (1 << row)); break;//is row 4 high
        default: return 0; break;//dont check any rows
    }
}
int Keypad::chkRow()
{
    /*Pre: nothing is passed in
     *Post: The row number that is
     *high is returned as an integer.
     */
    int row = 0;
    row = !(LPC_GPIO1->FIOPIN3 & (1 << 0))? 1:row;//is P1.28/row 1 high
    row = !(LPC_GPIO1->FIOPIN3 & (1 << 1))? 2:row;//is P1.26/row 2 high
    row = !(LPC_GPIO1->FIOPIN3 & (1 << 2))? 3:row;//is P1.25/row 3 high
    row = !(LPC_GPIO1->FIOPIN3 & (1 << 4))? 4:row;//is P1.24/row 4 high
    return row;
}
char Keypad::getChar()
{
    /*Pre: Keypad is initialized, and a char is declared
     *Present: The keypad is checked using setCol(), and
     *  chkRow(), and button is assigned the character
     *  associated with that button.
     *Post: A character is returned through function.
     */
    char button = '\0';//returned value initial set to
    int i,j; //used to point to character in 2d array

    for(i = 1; i<=4;i++)
    {
        setCol(i);//set the column to high
        j = chkRow();//finds the rows
        button = j!=0 && button!= '\0' ? KEYPAD_CHAR[4-j][4-i]: button;
    }
    if(j)
        delay_ms(100);//delay between characters entered.

    return button;
}
bool Keypad::getChar(char* button)
{
    /*Pre: Keypad is initialized, and point to char is given
     *Present: 1 The keypad is checked using
     *  setCol() & chkRow()
     *  2 button is assigned the character associated
     *  with that button through pointer
     *Post: 1 If a button is pressed a character is returned
     *through pointer and true is returned.
     * 2 Else false is returned
     */
    bool read = false;  // says if anything was read.
    int i,j; //used to point to character in 2d array
    for(i= 1; i<=4;i++)
    {
        setCol(i);//set the column to high
        j = chkRow();//finds the rows
        if(j!=0 && !read)//if button hasn't been assigned
        {
            *button = KEYPAD_CHAR[4-j][4-i];
            read= true; //a value was read
            delay_ms(200);//delay between characters entered.
        }
    }
    return read;
}
int Keypad::getVal()
{
    /*Pre: Keypad is initialized, and an int is declared
     *Present: The keypad is checked using setCol(), and
     *  chkRow(), and button number is returned
     *Post: An integer is returned through function.
     * if no button is pressed -999 us returned.
     */
    int button = -999;//returned value
    int i,j; //used to point to character in 2d array
    for(i= 1; i<=4;i++)
    {
        setCol(i);//set the column to high
        j = chkRow();//finds the rows
        //assign button if not already filled.
        button = ( j!=0 && button == -999 ) ? KEYPAD_VAL[4-j][4-i]: button;
    }
    if(j)
        delay_ms(200);//delay between characters entered.
    return button;
}
bool Keypad::getVal(int* button)
{
    /*Pre: Keypad is initialized, and an int is declared
     *Present: 1 The keypad is checked using setCol(), and
     *  chkRow()
     *  2 button number is returned via pointer
     *Post: An integer is returned through function if a
     *  button is pressed
     */
    bool read = false; //is true if button pressed
    int i,j; //used to point to character in 2d array
    for(i= 1; i<=4;i++)
    {
        setCol(i);//set the column to high
        j = chkRow();//finds the rows
        if(j!=0 && !read)
        {
            *button = KEYPAD_VAL[4-j][4-i];
            read = true;
        }
    }
    return read;
}
/*const char* Keypad::getValueAsString(int z)
{
    int x,y;
    mStr = "KP: ";
    while(z>0)
    {
        delay_ms(100);
        for(int x = 4; x>0; x--)
        {
            setKeypadCol(x);
            for(y = 4; y>0; y--)
            {
                if(KP.chkKeypadRow(y))
                {
                    mStr += KEY_PAD_VAL [y][z];
                    z++;
                }
            }
        }
    }
    return mStr();
}*/
//////////////////////////////////////////////////////////////////////////////
bool LCD::init()//CHECKED
{
    /*Pre:  none
     *Present: sets memory to write to spi1
     *Post: true is returned once memory is set.
     */
    spi1_Init();    //initialize the MCU to use SPI
    //set clock to 100kHz = (48Mhz)/(192*[4+1])
    LPC_SSP1->CPSR = 194;    //CPSR =  192
    LPC_SSP1->CR0 |= (1<<10);//SCR = 4
    LPC_GPIO0->FIODIR |= (1<<15); //Set p0.15 GPIO to output
    LPC_SSP1->CR0 |= (1<<6); //set CPOL = 1
    LPC_SSP1->CR0 |= (1<<7); //set CPHA = 1
    return true;
}
void LCD::setup(unsigned int brightness, unsigned int contrast, bool on)
{
    /*PRE: LCD is intialized
     *Present: 1 spi write enabled
     * 2 screen is on or off
     * 3 contrast set
     * 4 brightness set
     * 5 blink coursor
	 * 6 clear screen, coursor home
     * Post: screen is setup.
     */
    //begin write to spi
    LPC_GPIO0->FIOSET|= (1<<15); LPC_GPIO0->FIOCLR |= (1<<15);
    //turn on or off
    spi1_ExchangeByte(0xFE);
    if(on)
        spi1_ExchangeByte(0x41);
    else
        spi1_ExchangeByte(0x42);

    //set contrast
    spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x52);
    spi1_ExchangeByte(contrast);
    //setblack light
    spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x53);
    spi1_ExchangeByte(brightness);
    //set blinking coursor
    //spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x4B);
	//clear screen, set coursor to home
    spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x51);
    delay_ms(2);
    //end write to spi
    LPC_GPIO0->FIOSET|= (1<<15);
}
void LCD::on()  //CHECKED turns on screen
{
    /*Pre:  LCD is initialized
     *Present: 1 write to spi is enables
     * 2 turns on LCD through
     * 3 write is disabled
     *Post: 1 the screen is on.
     * 2 characters are saved in
     * LCD memory
     */
    //start write to SPI
    LPC_GPIO0->FIOSET|= (1<<15); LPC_GPIO0->FIOCLR |= (1<<15);
    //turn on
    spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x41);
    delay_ms(1);
    //end write to SPI
    LPC_GPIO0->FIOSET |= (1<<15); //make pin0.15 output high
    return;
}
void LCD::off() //CHECKED turns off screen
{
    /*Pre:  LCD is initialized
     *Present: 1 write to spi is enables
     * 2 turns off LCD through
     * 3 write is disabled
     *Post: 1 the screen is off
     * 2 characters are saved in
     * LCD memory
     */
    //start write to SPI
    LPC_GPIO0->FIOSET|= (1<<15); LPC_GPIO0->FIOCLR |= (1<<15);
    //turn off
    spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x42);
    delay_ms(1);
    //end write to SPI
    LPC_GPIO0->FIOSET |= (1<<15); //make pin0.15 output high
    return;
}
void LCD::clear()   //CHECKED clears the screen
{
    /*Pre:  LCD is initialized
     *Present: 1 write to spi is enables
     * 2 clears the LCD
     * 3 returns cursor to home
     * position(1,1)
     * 4 write is disabled
     *Post: 1 the screen is cleared
     * 2 present characters are
     * erased from memory.
     */
    //start write to SPI
    LPC_GPIO0->FIOSET|= (1<<15); LPC_GPIO0->FIOCLR |= (1<<15);
    //turn clear screen
    spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x51);
    delay_ms(2);
    //end write to SPI
    LPC_GPIO0->FIOSET |= (1<<15); //make pin0.15 output high
    return;
}
void LCD::clear(unsigned int row)        // CHECKED clears a row
{
    /*Pre:  LCD is initialized
     *Present: 1 write to spi is enables
     * 2 clears the specified row of LCD
     * 3 returns cursor to beginning of
     * row
     * 4 write is disabled
     *Post: the row is cleared
     * cursor is at beginning of row
     */
    if(row>0 && row<=4)
    {
        //start write to SPI
        LPC_GPIO0->FIOSET|= (1<<15); LPC_GPIO0->FIOCLR |= (1<<15);
        //set coursor
        spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x45);
        switch(row)
        {
        case 1: spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x45); spi1_ExchangeByte(0x14); break;
        case 2: spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x45); spi1_ExchangeByte(0x54); break;
        case 3: spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x45); spi1_ExchangeByte(0x28); break;
        default:spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x45); spi1_ExchangeByte(0x67);
                spi1_ExchangeByte(' ' ) ;//replace last character in row 4 with blank
                spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x45); spi1_ExchangeByte(0x67); break;
        }

        //back space
        for(int i = 0; i<21; i++)
        {
            spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x4E);
            delay_ms(330);
        }
        delay_ms(3);
        //end write to SPI
        LPC_GPIO0->FIOSET |= (1<<15); //make pin0.15 output high
    }
    return;
}
void LCD::B_Light(unsigned int level)    //CHECKED changes backlight brightness
{
    /*Pre:  LCD is initialized
     *Present: 1 write to spi is enables
     * 2 changes the back light brightness
     * depending on the integer recieved.
     * integer must be 0-8
     * 3 write is disabled
     *Post: the brightness changes.
     */
    if(level <=8 )
    {
        //start write to SPI
        LPC_GPIO0->FIOSET|= (1<<15); LPC_GPIO0->FIOCLR |= (1<<15);
        //set back light
        spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x53);
        spi1_ExchangeByte(level);
        delay_ms(1);
        //end write to SPI
        LPC_GPIO0->FIOSET |= (1<<15); //make pin0.15 output high
    }
    return;
}
void LCD::Contrast(unsigned int level)   //CHECKED changes screen contrast
{
    /*Pre:  LCD is initialized
     *Present:1 write to spi is enables
     * 2 changes the contrast of the LCD
     * depending on the integer recieved.
     * integer must be 0-15;
     * 3 write is disabled
     *Post: the contrast changes.
     */
    if(level <=50 )
    {
        //start write to SPI
        LPC_GPIO0->FIOSET|= (1<<15); LPC_GPIO0->FIOCLR |= (1<<15);
        //set contrast
        spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x52);
        spi1_ExchangeByte(level);
        delay_ms(1);
        //end write to SPI
        LPC_GPIO0->FIOSET |= (1<<15); //make pin0.15 output high
    }
    return;
}
void LCD::write(const char* words, unsigned int row) //writes to a specific row.
{
    /*Pre:  LCD is initialized
     *Present: 1 write to spi is enables
     * 2 courser moved to start of row
     * 3 writes an array of char up to 20
     * 4 write is disabled
     *Post: row is updated with new chars
     */
    int i = 0;
    if(row >= 0 && row <4)
    {
        //start write to SPI
        LPC_GPIO0->FIOSET|= (1<<15); LPC_GPIO0->FIOCLR |= (1<<15);
        //set position
        switch(row)
        {
            case 1: spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x45); spi1_ExchangeByte(0x40); break;
            case 2: spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x45); spi1_ExchangeByte(0x14);break;
            case 3: spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x45); spi1_ExchangeByte(0x54);break;
            default:spi1_ExchangeByte(0xFE); spi1_ExchangeByte(0x45); spi1_ExchangeByte(0x00);break;
        }
        while(words[i]!='\0' && i<20)
        {
            spi1_ExchangeByte(words[i]);
            i++;
        }
        //end write to SPI
        LPC_GPIO0->FIOSET |= (1<<15); //make pin0.15 output high
    }
    return;
}
void LCD::write(const char* words)          //writes starting from home position
{
    /*Pre:  LCD is initialized
     *Present: 1 write to spi is enables
     * 2 clear() is called
     * 3 writes an array of char up to 80
     * 4 write is disabled
     *Post: screen is updated with new chars
     */
    int i = 0;

    //clears the screen
    clear();

    //start write to SPI
    LPC_GPIO0->FIOSET|= (1<<15); LPC_GPIO0->FIOCLR |= (1<<15);
    while(words[i]!='\0' && i<80)
    {
        spi1_ExchangeByte(words[i++]);
        //i++;
    }
    //end write to SPI
    LPC_GPIO0->FIOSET |= (1<<15); //make pin0.15 output high
    return;
}
//////////////////////////////////////////////////////////////////////////////
bool TemperatureSensor::init()
{
    const unsigned char temperatureCfgRegPtr = 0x01;
    const unsigned char expectedBitsThatAreNotZero = 0x60; // R1:R0 bits are 1s at startup

    const unsigned char cfgRegByte0 = readReg(temperatureCfgRegPtr);
    return (0 != (cfgRegByte0 & expectedBitsThatAreNotZero));
}
float TemperatureSensor::getCelsius()
{
    // Get signed 16-bit data of temperature register pointer
    const unsigned char temperatureRegsiterPtr = 0x00;
    signed short temperature = get16BitRegister(temperatureRegsiterPtr);

    // Temperature data is in bits 15:3 which contains signed 16-bit data
    temperature /= 16;

    // Each bit is of 0.0625 degree per bit resolution
    return (0.0625F * temperature);
}
float TemperatureSensor::getFarenheit()
{
    return (getCelsius() * 9.0F / 5) + 32;
}
const char* TemperatureSensor::getValueAsString()
{
    const unsigned char celciusSymbol = 176;
    float t = getCelsius();
    int sig = t;
    int dec = (t - sig) * 100;

    mStr.printf("TS: %i.%i%cC (%i%cF)", sig, dec, celciusSymbol, (int)getFarenheit(), celciusSymbol);
    return mStr();
}
/*const char* TemperatureSensor::getValueAsString()
{
    //const unsigned char celciusSymbol = 176;
    float t = getCelsius();
    int sig = t;
    int dec = (t - sig) * 100;

    mStr.printf("TS: %i.%2iC", sig, dec);
    return mStr();
}*/
