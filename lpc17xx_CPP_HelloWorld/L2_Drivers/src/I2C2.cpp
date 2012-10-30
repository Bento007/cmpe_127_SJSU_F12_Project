#include "I2C2.hpp"
#include "LPC17xx.h"




/**
 * IRQ Handler needs to be enclosed in extern "C" because this is C++ file, and
 * we don't want C++ to "mangle" this function name.
 * This ISR Function need needs to be named precisely to override "WEAK" ISR
 * handler defined at cr_startup_lpc175x.cpp
 */
extern "C"
{
    void I2C2_IRQHandler()
    {
        I2C2::getInstance().handleInterrupt();
    }
}



bool I2C2::init(unsigned int speedInKhz)
{
    LPC_PINCON->PINSEL0 &= ~(0xF << 20);  // Clear
    LPC_PINCON->PINSEL0 |=  (0xA << 20);  // Enable I2C Pins: SDA, SCL

    // Set I2C2 Peripheral Clock divider to 4
    LPC_SC->PCLKSEL1 &= ~(3 << 20);
    const unsigned int pclk = getCpuClock() / 4;

    return I2C_Base::init(pclk, speedInKhz);
}

I2C2::I2C2() : I2C_Base((LPC_I2C_TypeDef*) LPC_I2C2_BASE)
{

}
