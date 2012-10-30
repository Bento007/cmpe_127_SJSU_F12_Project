#include "spi1.h"
#include "sysConfig.h"

/**
 * TODO Use the DMA for SSP transfers.
 * This will decrease code size as many macros are used to do fast 256 byte transfers
 */
#define USE_DMA     0

void spi1_SetMaxClockMhz(unsigned int maxClockMhz)
{
    unsigned int divider = 2;
    const unsigned int cpuClockMhz = getCpuClock() / (1000 * 1000UL);

    // Keep scaling down divider until calculated is higher
    // Example:
    // 60, need 18
    // 60/2   18 < 30 YES
    // 60/4   18 < 15 NO
    while(maxClockMhz < (cpuClockMhz / divider) && divider <= 254)
    {
        divider += 2;
    }

    LPC_SSP1->CPSR = divider;
}

void spi1_Init()
{
    LPC_SC->PCONP |= (1 << 10);     // SPI1 Power Enable
    LPC_SC->PCLKSEL0 &= ~(3 << 20); // Clear clock Bits
    LPC_SC->PCLKSEL0 |=  (1 << 20); // CLK / 1

    // Clear bits 19:18:17:16:15:14 and set 10.10.10 to use as SPI1 pins
    LPC_PINCON->PINSEL0 &= ~( (3 << 14) | (3 << 16) | (3 << 18) );
    LPC_PINCON->PINSEL0 |=  ( (2 << 14) | (2 << 16) | (2 << 18) );

    LPC_SSP1->CR0 = 7;          // 8-bit mode
    LPC_SSP1->CR1 = (1 << 1);   // Enable SSP as Master

#if USE_DMA
    // Initialize DMA capability:
    LPC_SSP1->DMACR = (1 << 0) | (1 << 1);

    // Enable power to DMA:
    LPC_SC->PCONP |= (1 << 29);
    LPC_GPDMA->DMACConfig    |= (1 << 0); // GP-DMA Enable
#endif

    spi1_SetMaxClockMhz(12);
}

#if USE_DMA
void spi1_Receive256Bytes(unsigned char* pDest)
{
    // Clear any pending interrupt and errors:
    LPC_GPDMA->DMACIntErrClr = 0xFF;
    LPC_GPDMA->DMACIntTCClear = 0xFF;

    // Set source to SSP1 Data register and destination to the memory pointer
    LPC_GPDMACH0->DMACCSrcAddr  = &(LPC_SSP1->DR);
    LPC_GPDMACH0->DMACCDestAddr = pDest;

    const unsigned short transferSize = (512 << 0);
    const unsigned char srcBurstSize  = (0 << 12);
    const unsigned char dstBurstSize  = (0 << 15);
    const unsigned char srcWidthOneByte = (0 << 18);
    const unsigned char dstWidthOneByte = (0 << 21);
    const unsigned char srcDoNotIncrement = (0 << 26);
    const unsigned char dstIncrement      = (1 << 27);

    // Set linked list to NULL and configure the DMA transfer through CONTROL register
    LPC_GPDMACH0->DMACCLLI = 0;
    LPC_GPDMACH0->DMACCControl = (transferSize | srcBurstSize | dstBurstSize |
                                    srcWidthOneByte | dstWidthOneByte |
                                    srcDoNotIncrement | dstIncrement);

    LPC_GPDMA->DMACEnbldChns |= (1 << 0); // Enable DMA Channel 0

    // Enable the channel, set source peripheral to SSP1 RX, and transfer type to Peripheral to Memory
    LPC_GPDMACH0->DMACCConfig = (1 << 0) | (3 << 1) |  (2 << 11);
}

void spi1_Send256Bytes(unsigned char* pSrc)
{
}
#else

/**
 * This macro waits until RX FIFO is half full (4 bytes)
 * then it reads 4 bytes and queues off 4 more bytes of transfer
 */
#define spi1_Rx4BytesTx4Bytes(pByteArray)       \
        while(! (LPC_SSP1->RIS & (1 << 2)));    \
        *pByteArray++ = LPC_SSP1->DR;           \
        LPC_SSP1->DR = 0xFF;                    \
        *pByteArray++ = LPC_SSP1->DR;           \
        LPC_SSP1->DR = 0xFF;                    \
        *pByteArray++ = LPC_SSP1->DR;           \
        LPC_SSP1->DR = 0xFF;                    \
        *pByteArray++ = LPC_SSP1->DR;           \
        LPC_SSP1->DR = 0xFF

/**
 * This macro calls the macro above 4 times to do 16 byte transfers
 */
#define spi1_Rx16BytesTx16Bytes(p256Bytes)      \
        spi1_Rx4BytesTx4Bytes(p256Bytes);       \
        spi1_Rx4BytesTx4Bytes(p256Bytes);       \
        spi1_Rx4BytesTx4Bytes(p256Bytes);       \
        spi1_Rx4BytesTx4Bytes(p256Bytes)

void spi1_Receive256Bytes(unsigned char* p256Bytes)
{
    // Let the SSP roll with 8 byte initial transfer
    LPC_SSP1->DR = 0xFF;
    LPC_SSP1->DR = 0xFF;
    LPC_SSP1->DR = 0xFF;
    LPC_SSP1->DR = 0xFF;
    LPC_SSP1->DR = 0xFF;
    LPC_SSP1->DR = 0xFF;
    LPC_SSP1->DR = 0xFF;
    LPC_SSP1->DR = 0xFF;

    // Now as soon as Rx FIFO shows half full, read 4 bytes and send 4 bytes through macros
    spi1_Rx16BytesTx16Bytes(p256Bytes); // 16 Bytes
    spi1_Rx16BytesTx16Bytes(p256Bytes); // 32 Bytes
    spi1_Rx16BytesTx16Bytes(p256Bytes); // 48 Bytes
    spi1_Rx16BytesTx16Bytes(p256Bytes); // 64 Bytes
    spi1_Rx16BytesTx16Bytes(p256Bytes); // 80 Bytes
    spi1_Rx16BytesTx16Bytes(p256Bytes); // 96 Bytes
    spi1_Rx16BytesTx16Bytes(p256Bytes); // 112 Bytes
    spi1_Rx16BytesTx16Bytes(p256Bytes); // 128 Bytes
    spi1_Rx16BytesTx16Bytes(p256Bytes); // 144 Bytes
    spi1_Rx16BytesTx16Bytes(p256Bytes); // 160 Bytes
    spi1_Rx16BytesTx16Bytes(p256Bytes); // 176 Bytes
    spi1_Rx16BytesTx16Bytes(p256Bytes); // 192 Bytes
    spi1_Rx16BytesTx16Bytes(p256Bytes); // 208 Bytes
    spi1_Rx16BytesTx16Bytes(p256Bytes); // 224 Bytes
    spi1_Rx16BytesTx16Bytes(p256Bytes); // 240 Bytes

    spi1_Rx4BytesTx4Bytes(p256Bytes); /* Read 244 bytes */
    spi1_Rx4BytesTx4Bytes(p256Bytes); /* Read 248 bytes */

    /* SSP should have 8 bytes still pending at this point */
    while(! (LPC_SSP1->RIS & (1 << 2)));
    *p256Bytes++ = LPC_SSP1->DR;
    *p256Bytes++ = LPC_SSP1->DR;
    *p256Bytes++ = LPC_SSP1->DR;
    *p256Bytes++ = LPC_SSP1->DR;

    while(! (LPC_SSP1->RIS & (1 << 2)));
    *p256Bytes++ = LPC_SSP1->DR;
    *p256Bytes++ = LPC_SSP1->DR;
    *p256Bytes++ = LPC_SSP1->DR;
    *p256Bytes++ = LPC_SSP1->DR;
}

#define spi1_send4Bytes(p256Bytes) \
            while(! (LPC_SSP1->RIS & (1 << 3)));    \
            LPC_SSP1->DR = *p256Bytes++;            \
            LPC_SSP1->DR = *p256Bytes++;            \
            LPC_SSP1->DR = *p256Bytes++;            \
            LPC_SSP1->DR = *p256Bytes++
#define spi1_send16Bytes(p256Bytes)         \
            spi1_send4Bytes(p256Bytes);     \
            spi1_send4Bytes(p256Bytes);     \
            spi1_send4Bytes(p256Bytes);     \
            spi1_send4Bytes(p256Bytes)

void spi1_Send256Bytes(const unsigned char* p256Bytes)
{
    spi1_send16Bytes(p256Bytes); // 16 Bytes
    spi1_send16Bytes(p256Bytes); // 32 Bytes
    spi1_send16Bytes(p256Bytes); // 48 Bytes
    spi1_send16Bytes(p256Bytes); // 64 Bytes
    spi1_send16Bytes(p256Bytes); // 80 Bytes
    spi1_send16Bytes(p256Bytes); // 96 Bytes
    spi1_send16Bytes(p256Bytes); // 112 Bytes
    spi1_send16Bytes(p256Bytes); // 128 Bytes

    spi1_send16Bytes(p256Bytes); // 144 Bytes
    spi1_send16Bytes(p256Bytes); // 160 Bytes
    spi1_send16Bytes(p256Bytes); // 176 Bytes
    spi1_send16Bytes(p256Bytes); // 192 Bytes
    spi1_send16Bytes(p256Bytes); // 208 Bytes
    spi1_send16Bytes(p256Bytes); // 224 Bytes
    spi1_send16Bytes(p256Bytes); // 240 Bytes
    spi1_send16Bytes(p256Bytes); // 256 Bytes

    /* Wait for SSP to finish */
    while(LPC_SSP1->SR & (1 << 4));

    /* Important: Empty FIFO otherwise Rx FIFO will show full */
    char dummy = LPC_SSP1->DR;
    dummy = LPC_SSP1->DR;
    dummy = LPC_SSP1->DR;
    dummy = LPC_SSP1->DR;

    dummy = LPC_SSP1->DR;
    dummy = LPC_SSP1->DR;
    dummy = LPC_SSP1->DR;
    dummy = LPC_SSP1->DR;
}
#endif /* If not using DMA */

