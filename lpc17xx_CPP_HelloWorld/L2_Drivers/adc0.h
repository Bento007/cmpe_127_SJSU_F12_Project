/**
 * @file    adc0.h
 * @ingroup Drivers
 */
#ifndef ADC0_H_
#define ADC0_H_
#ifdef __cplusplus
extern "C" {
#endif


#include "LPC17xx.h"
#include "sysConfig.h"


/**
 * Initializes the ADC Peripheral
 * @note The PIN that will be used by ADC needs to be selected using PINSEL externally
 */
inline void adc0_initialize()
{
    // Clear PDN bit before powering up ADC0
    LPC_SC->PCONP |= (1 << 12);     // Power up ADC
    LPC_ADC->ADCR = (1 << 21);      // Enable ADC

    LPC_SC->PCLKSEL0 &= ~(3 << 24); // ADC Clock = CPU Clock / 4

    // Calculate and set prescalar for ADC.  13Mhz is maximum value
    const unsigned int adcClock = (getCpuClock() / 4);
    const unsigned int maxAdcClock = (12 * 1000UL * 1000UL);
    for(int i=0; i < 255; i+=2) {
        if( (adcClock / i) < maxAdcClock) {
            LPC_ADC->ADCR |= (i << 8);
            break;
        }
    }
}

/**
 * Gets an ADC reading from a channel number between 0 - 7
 * @returns 12-bit ADC value read from the ADC.
 */
inline unsigned short adc0_getReading(unsigned char channelNum)
{
    if(channelNum >= 8) {
        return 0;
    }

    // Clear previously selected channel
    LPC_ADC->ADCR &= ~((0xFF) | (0x7 << 24));

    // Set the channel number and start the conversion now
    LPC_ADC->ADCR |= (1 << channelNum);
    LPC_ADC->ADCR |= (1 << 24);

    // Wait for conversion to complete
    while(! (LPC_ADC->ADGDR & (1 << 31)));

    // Pick up the result from bits 15:4
    unsigned short result = LPC_ADC->ADGDR & 0xFFFF;
    result >>= 4;

    return result;
}


#ifdef __cplusplus
}
#endif
#endif /* ADC0_H_ */
