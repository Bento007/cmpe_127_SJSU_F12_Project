#include "sysConfig.h"      // CPU Clock Configuration
#include "LPC17xx.h"        // LPC MemoryMap
#include "uart0_min.h"      // Uart0 init
#include "io_functions.h"   // Uart0 IO connect to stdio
#include <stdio.h>          // setvbuf & printf


void configureFlashAccelerator(unsigned int clock)
{
    const unsigned int clockMhz = clock / (1000 * 1000);

    // Erase flash bits:
    LPC_SC->FLASHCFG &= (0xF << 12);

    // Flash accelerator parameters depends on CPU clock to optimize program code read (CPU instructions)
    switch(clockMhz)
    {
        case  0 ... 20  : LPC_SC->FLASHCFG |= (0 << 12); break;
        case 21 ... 40  : LPC_SC->FLASHCFG |= (1 << 12); break;
        case 41 ... 60  : LPC_SC->FLASHCFG |= (2 << 12); break;
        case 61 ... 80  : LPC_SC->FLASHCFG |= (3 << 12); break;
        case 81 ... 100 : LPC_SC->FLASHCFG |= (4 << 12); break;
        default:
            LPC_SC->FLASHCFG = (5 << 12); /* works for all clock settings */
            break;
    }
}

/**
 * Initializes the minimal system including CPU Clock, UART, and Flash accelerator
 */
void lowLevelInitialize()
{
    // Configure System Clock based on desired clock rate @ sysConfig.h
    extern void configureSystemClock();
    configureSystemClock();
    const unsigned int cpuClock = getCpuClock();
    configureFlashAccelerator(cpuClock);

    // Setup UART with minimum I/O functions
    uart0Init(UART0_DEFAULT_RATE_BPS, (cpuClock / 4));
    stdio_SetOutputCharFunction(uart0Putchar);
    stdio_SetInputCharFunction(uart0Getchar);

    /**
     *  If buffers are set to 0, so printf/scanf will behave correctly!
     *  If not set, then printf/scanf will have weird buffering/flushing effects
     */
    setvbuf(stdout, 0, _IONBF, 0);
    setvbuf(stdin, 0, _IONBF, 0);

    /**
     * Print Boot Info
     */
    printf("\nSystem Boot @ %u.%u Mhz (Build: %s, %s)\n",
            cpuClock / (1000 * 1000), cpuClock % (1000 * 1000), // Poor man's method of floating point printf
            __DATE__, __TIME__);
}

