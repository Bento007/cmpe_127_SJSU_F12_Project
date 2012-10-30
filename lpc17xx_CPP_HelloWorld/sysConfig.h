/**
 * @file sysConfig.h
 * @brief This file provides the configurable paramters for the CPU Clock
 * 			Note that the PLL is always enabled, and so the desired CPU_CLOCK
 * 			should be chosen based on system needs.
 */
#ifndef SYSCONFIG_H_
#define SYSCONFIG_H_
#ifdef __cplusplus
extern "C" {
#endif


/**
 * @returns actual System clock as calculated from PLL and Oscillator selection
 * @note The DESIRED_CPU_CLOCK macro defines "Desired" CPU clock, and doesn't guarantee
 *          this clock rate.  This function returns actual CPU clock of the system.
 */
unsigned int getCpuClock();


/**
 * Select the clock source:
 * - 0: Internal Clock: 4Mhz  1% Tolerance
 * - 1: External Clock: 12Mhz
 * - 2: RTC Clock     : 32.768Khz
 */
#define CLOCK_SOURCE		0


#define INTERNAL_CLOCK		(4  * 1000 * 1000UL)    ///< Do not change, this is the same on all LPC17XX
#define EXTERNAL_CLOCK      (12 * 1000 * 1000UL)    ///< Change according to your board configuration
#define RTC_CLOCK           (32768UL)               ///< Change according to your board configuration


#define DESIRED_CPU_CLOCK	(48 * 1000 * 1000UL)        ///< Define the CPU speed you desire, must be between 1-100Mhz
#define FALLBACK_CPU_CLOCK  (24 * 1000 * 1000UL)        ///< Do not change.  This is the fallback CPU speed if DESIRED_CPU_CLOCK cannot be attained
#define PERIPHERAL_CLOCK	(DESIRED_CPU_CLOCK / 4UL)   ///< Peripheral clock is setup by default to be CPU Clock divided by 4
#define TIMER0_US_PER_TICK  (10)                        ///< Microseconds per tick for Timer0 that is used by FreeRTOS


/**
 * Set printf options - Do a clean build after changing this option
 *  - 0 : Full printf from stdio.h --> Slowest : Uses more code size + RAM
 *  - 1 : iprintf from stdio.h     --> Faster  : -16K Reduced  code size but without floating point support
 *  - 2 : Custom printf            --> Fastest : -21K Smallest code size but not complaint with all printf options
 */
#define USE_REDUCED_PRINTF        2
#define UART0_DEFAULT_RATE_BPS    38400	 ///< UART0 is configured at this BPS by Startup Code - before main()







#ifdef __cplusplus
}
#endif
#endif /* SYSCONFIG_H_ */
