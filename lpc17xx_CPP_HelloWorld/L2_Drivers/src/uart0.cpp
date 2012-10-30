#include "uart0.hpp"
#include "LPC17xx.h"    // LPC_UART0_BASE
#include "sysConfig.h"  // getSystemClock()


/**
 * IRQ Handler needs to be enclosed in extern "C" because this is C++ file, and
 * we don't want C++ to "mangle" our function name.
 * This ISR Function need needs to be named precisely to override "WEAK" ISR
 * handler defined at cr_startup_lpc175x.cpp
 */
extern "C"
{
    void UART0_IRQHandler()
    {
        UART0::getInstance().handleInterrupt();
    }
}


char UART0::getcharIntrDriven(char unused)
{
    char c = 0;
    UART0::getInstance().getChar(&c);
    return c;
}
char UART0::putcharIntrDriven(char theChar)
{
    return UART0::getInstance().putChar(theChar);
}

bool UART0::init(unsigned int baudRate, int rxQSize, int txQSize)
{
    // Configure PINSEL for UART0
    LPC_PINCON->PINSEL0 &= ~(0xF << 4); // Clear values
    LPC_PINCON->PINSEL0 |=  (0x5 << 4); // Set values for UART0 Rx/Tx

    // Set UART0 Peripheral Clock divider to 4
    LPC_SC->PCLKSEL0 &= ~(3 << 6);
    const unsigned int pclk = getCpuClock() / 4;

    return UART_Base::init(pclk, baudRate, rxQSize, txQSize);
}

UART0::UART0() : UART_Base((unsigned int*)LPC_UART0_BASE)
{
    // Nothing to do here other than handing off LPC_UART0_Base address to UART_Base
}
