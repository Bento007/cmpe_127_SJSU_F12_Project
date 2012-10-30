/**
 * @file    spi1.h
 * @ingroup Drivers
 */
#ifndef SPI1_H_
#define SPI1_H_
#ifdef __cplusplus
extern "C" {
#endif
#include "LPC17xx.h"



/**
 * Sets SPI Clock speed
 * @param maxClockMhz   The maximum speed of this SPI in Megahertz
 * @note The speed may be set lower to maxClockMhz if it cannot be attained.
 */
void spi1_SetMaxClockMhz(unsigned int maxClockMhz);

/**
 * Initializes SPI 1
 * Configures CLK, MISO, MOSI pins with a slow SCK speed
 */
void spi1_Init();

/**
 * Exchanges a byte over SPI bus
 * @param out   The byte to send out
 * @returns     The byte received over SPI
 */
inline char spi1_ExchangeByte(char out)
{
    LPC_SSP1->DR = out;
    while(LPC_SSP1->SR & (1 << 4)); // Wait until SSP is busy
    return LPC_SSP1->DR;
}

/**
 * Sends a block of 256 bytes pointed by @param p256Bytes
 * Unlike sending byte by byte, this function uses GP-DMA if enabled, or
 * using SPI FIFO to speed up the transfer.
 */
void spi1_Receive256Bytes(unsigned char* p256Bytes);

/**
 * Receives 256 bytes and saves it to @param p256Bytes
 * Unlike sending byte by byte, this function uses GP-DMA if enabled, or
 * using SPI FIFO to speed up the transfer.
 */
void spi1_Send256Bytes(const unsigned char* p256Bytes);



#ifdef __cplusplus
}
#endif
#endif /* SPI1_H_ */
