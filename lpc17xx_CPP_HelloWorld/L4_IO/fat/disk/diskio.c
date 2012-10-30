#include "diskio.h"
#include "spi_flash.h"
#include "sd.h"



static xSemaphoreHandle *pSpiSemaphore = 0;
void diskio_initializeSPIMutex(xSemaphoreHandle *pSemaphore)  {   pSpiSemaphore = pSemaphore;      }

/**
 * A very smart, but (slightly) ugly macro, but saves us lots of duplicate code.
 * The benefits of this macro outweigh the negatives because this macro will obtain
 * the semaphore only when FreeRTOS is started, and give it back as well.
 * If FreeRTOS is not started, this will not make the FreeRTOS API call at all.
 *
 * @warning DO NOT USE RETURN statement in this section otherwise it will break it!!
 */
#define MUTEX_SECTION() \
    for(char rtosStarted = (taskSCHEDULER_RUNNING == xTaskGetSchedulerState()),         \
                lock = rtosStarted ? xSemaphoreTake(*pSpiSemaphore, portMAX_DELAY) : 1; \
                0 != lock; \
                lock = rtosStarted ? !xSemaphoreGive(*pSpiSemaphore) : 0)
                /* If xSemaphoreGive() returns true, convert to false on purpose to quit loop */


DSTATUS disk_initialize(BYTE drv)
{
    DSTATUS status = RES_PARERR;

    MUTEX_SECTION()
    {
        switch(drv)
        {
            case flashDriveNum: status = flash_Initialize();    break;
            case sdDriveNum:    status = sd_initialize();       break;
            default: status = RES_PARERR;    break;
        }
    }

    return status;
}

DSTATUS disk_status(BYTE drv)
{
    DSTATUS status = RES_PARERR;

    MUTEX_SECTION()
    {
        switch(drv)
        {
            // Flash memory is always good to go!
            case flashDriveNum: status = RES_OK;        break;
            case sdDriveNum:    status = sd_status();   break;
            default: status = RES_PARERR;    break;
        }
    }

    return status;
}

DRESULT disk_read (BYTE drv,       BYTE *buff, DWORD sector, BYTE count)
{
    DSTATUS status = RES_PARERR;

    MUTEX_SECTION()
    {
        switch(drv)
        {
            case flashDriveNum: status = flash_ReadSector(buff, sector, count);    break;
            case sdDriveNum:    status = sd_read(buff, sector, count);  break;
            default:            status = RES_PARERR;                    break;
        }
    }

    return status;
}

DRESULT disk_write(BYTE drv, const BYTE *buff, DWORD sector, BYTE count)
{
    DSTATUS status = RES_PARERR;

    MUTEX_SECTION()
    {
        switch(drv)
        {
            case flashDriveNum: status = flash_WriteSector(buff, sector, count); break;
            case sdDriveNum:    status = sd_write(buff, sector, count);  break;
            default: status = RES_PARERR;    break;
        }
    }

    return status;
}

DRESULT disk_ioctl(BYTE drv, BYTE ctrl,void *buff)
{
    DSTATUS status = RES_PARERR;

    MUTEX_SECTION()
    {
        switch(drv)
        {
            case flashDriveNum: status = flash_ioctl(ctrl, buff);   break;
            case sdDriveNum:    status = sd_ioctl(ctrl, buff);      break;
            default:            status = RES_PARERR; break;
        }
    }

    return status;
}
