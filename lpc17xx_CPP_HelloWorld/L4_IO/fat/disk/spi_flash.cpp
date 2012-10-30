#include "spi_flash.h"
#include "spi1.h"
#include "disk_defines.h"
#include "LPC17xx.h"
#include "fat/ff.h"         // FR_OK and FR_DISK_ERR
#include <stdio.h>          // puts()

/// The GPIO Pin number of the Flash Memory's chip-select (CS) signal
#define FLASH_CS_SIGNAL      (1 << 16)

inline void flashDeselect()
{
    LPC_GPIO0->FIOSET = FLASH_CS_SIGNAL;
}
/**
* flashDeslect() is called twice because there is a minimum time of 50ns
* between flashDeselect() and flashSelect() and this may be violated if
* this macro is called back to back.
*/
inline char flashSelect()
{
    flashDeselect();
    flashDeselect();
    LPC_GPIO0->FIOCLR = FLASH_CS_SIGNAL;
    return 0;
}
void flash_InitializeSignals()
{
    LPC_PINCON->PINSEL1 &= ~(3 << 0);
    LPC_GPIO0->FIODIR |= FLASH_CS_SIGNAL;
    flashDeselect();
}
inline char spiExchangeByte(char b)
{
    return spi1_ExchangeByte(b);
}






#define FLASH_MEMORY_SIZE   (1 * 1024 * 1024)   ///< Flash Memory Size in Bytes
#define FLASH_SECTOR_SIZE   (512)               ///< Minimum sector size that works with FATFS
#define FLASH_PAGE_SIZE     (256)               ///< The page size in bytes of this Flash Memory

/**
 * OPCODES of this Flash Memory
 */
typedef enum {
    opCode_StatusReg = 0xD7,
    opCode_GetSignature = 0x9F,
    opCode_readContinousMemRead = 0xE8,

    opCode_PageErase = 0x81,
    opCode_Buffer1Write = 0x84,
    opCode_buffer1ToMemWithoutBuiltInErase = 0x88
}FlashOpCodeType;

/**
 * This union structure is used to access the bytes of an integer
 */
typedef union
{
   /// Full Address that overlaps with the structure below
   unsigned int fullAddr;

   struct {
       unsigned char B07_B00;
       unsigned char B15_B08;
       unsigned char B23_B16;
       unsigned char B31_B24;
   }byte;
}FlashAddressType;


/**
 * Macro that selects and de-selects the Flash chip-select signal
 */
#define CHIP_SELECT_OP()    for(unsigned int ___i=flashSelect();\
                                    ___i!=1; ___i++, \
                                    flashDeselect())




/** @{ Private Functions used at this file */
    static void waitUntilNotBusy();
    static void flash_WritePage(const unsigned char* pData, const FlashAddressType addr);
    inline void flash_SendOpCodeAndAddr(const FlashOpCodeType opcode, const FlashAddressType addr)
    {
        spiExchangeByte(opcode);
        spiExchangeByte(addr.byte.B23_B16);
        spiExchangeByte(addr.byte.B15_B08);
        spiExchangeByte(addr.byte.B07_B00);
    }
/** @} */



DSTATUS flash_Initialize()
{
    waitUntilNotBusy();

    char statusReg = 0;
    CHIP_SELECT_OP()
    {
        spiExchangeByte(opCode_StatusReg);
        statusReg = spiExchangeByte(0xFF);
    }

    const unsigned char expectedSignatureByte1 = 0x1F;
    unsigned char actualSignature = 0x00;
    CHIP_SELECT_OP()
    {
        spiExchangeByte(opCode_GetSignature);
        actualSignature = spiExchangeByte(0xFF);
    }

    /**
     * If Page Size is not 256 bytes, make the page size 256 bytes
     * @warning THIS CAN ONLY BE CHANGED ONCE, and Page size cannot
     * be configured back to 264 bytes.
     * After Page is configured to be 256 bytes per page, a Power-Cycle is required.
     * This procedure will only occur once during the Board's life-span.
     */
    const char PageSize256BytesBitMask = (1 << 0);
    if(! (statusReg & PageSize256BytesBitMask))
    {
        CHIP_SELECT_OP()
        {
            spiExchangeByte(0x3D);
            spiExchangeByte(0x2A);
            spiExchangeByte(0x80);
            spiExchangeByte(0xA6);
        }
        puts("----> POWER OFF AND POWER ON CPU TO INITIALIZE FLASH MEMORY!!! <-------");
        while(1);
    }

    return (expectedSignatureByte1 == actualSignature) ? FR_OK : FR_DISK_ERR;
}

DRESULT flash_ReadSector(unsigned char* pData, int sectorNum, int sectorCount)
{
    FlashAddressType addr;
    addr.fullAddr = (sectorNum * FLASH_SECTOR_SIZE);
    const unsigned char dummyByte = 0xFF;

    waitUntilNotBusy();
    CHIP_SELECT_OP()
    {
        flash_SendOpCodeAndAddr(opCode_readContinousMemRead, addr);
        spiExchangeByte(dummyByte);
        spiExchangeByte(dummyByte);
        spiExchangeByte(dummyByte);
        spiExchangeByte(dummyByte);

        for(int i = 0; i < sectorCount; i++) {
            spi1_Receive256Bytes(pData);
            pData += 256;
            spi1_Receive256Bytes(pData);
            pData += 256;
        }
    }

    return RES_OK;
}

DRESULT flash_WriteSector(const unsigned char* pData, int sectorNum, int sectorCount)
{
    FlashAddressType addr;
    addr.fullAddr = (sectorNum * FLASH_SECTOR_SIZE);

    /**
     * Flash Memory works by pages of 256 bytes each, so need to
     * write two pages that account for one sector of 512 bytes
     */
    for(int i = 0; i < sectorCount; i++)
    {
        flash_WritePage(pData, addr);
        pData += FLASH_PAGE_SIZE;
        addr.fullAddr += FLASH_PAGE_SIZE;

        flash_WritePage(pData, addr);
        pData += FLASH_PAGE_SIZE;
        addr.fullAddr += FLASH_PAGE_SIZE;
    }

    return RES_OK;
}

DRESULT flash_ioctl(BYTE ctrl,void *buff)
{
    DRESULT status = RES_PARERR;

    switch(ctrl)
    {
        case CTRL_POWER:
        case CTRL_LOCK:
        case CTRL_EJECT:
            status = RES_OK;
            break;

        /**
         * Flush any data, but flash memory never has "pending data" to flush because
         * when we read, we always go through waitUntilNotBusy();
         * Having waitUntilNotBusy() however will prevent File System from being corrupted
         * at the expense of slower performance
         */
        case CTRL_SYNC:
            waitUntilNotBusy();
            status = RES_OK;
            break;

        // Used by mkfs() while formatting the memory
        case GET_SECTOR_COUNT:
            *(DWORD*) buff = (DWORD) ((FLASH_MEMORY_SIZE / FLASH_SECTOR_SIZE));
            status = RES_OK;
            break;

        case GET_SECTOR_SIZE:
            *(WORD*) buff = FLASH_SECTOR_SIZE;
            status = RES_OK;
            break;

        // Used by mkfs() while aligning memory
        case GET_BLOCK_SIZE:
            *(DWORD*) buff = 1; /* Block size is unknown */
            status = RES_OK;
            break;

        /*
        case CTRL_ERASE_SECTOR:
            status = RES_OK;
            break;
        */

        default:
            status = RES_PARERR;
            break;
    }
    return status;
}






static void waitUntilNotBusy()
{
    const char busyBit = (1 << 7);

    CHIP_SELECT_OP()
    {
        spiExchangeByte(opCode_StatusReg);

        // Device is busy if Bit7 is a 0
        while(! (spiExchangeByte(0xFF) & busyBit))
        {
            ;
        }
    }
}

static void flash_WritePage(const unsigned char* pData, const FlashAddressType addr)
{
    waitUntilNotBusy();
    CHIP_SELECT_OP()
    {
        flash_SendOpCodeAndAddr((FlashOpCodeType)0x82, addr);
        spi1_Send256Bytes(pData);
    }
}


