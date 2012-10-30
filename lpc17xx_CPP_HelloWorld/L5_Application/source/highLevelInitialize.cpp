#include <stdio.h>           // printf

#include "sysConfig.h"      // CPU Clock Configuration
#include "handles.h"        // Get instance of SPI Handles to set for DISK IO Layer
#include "utilities.h"      // printMemoryInfo()

#include "fat/disk/sd.h"        // Initialize SD Card Pins for CS, WP, and CD
#include "fat/disk/spi_flash.h" // Initialize Flash CS pin
#include "filelogger.hpp"

#include "rtc.h"             // RTC init
#include "I2C2.hpp"          // I2C1 init
#include "adc0.h"            // ADC0 init
#include "spi1.h"            // SPI-1 init
#include "storage.hpp"       // Mount Flash & SD Storage
#include "io.hpp"


typedef void (*voidFuncPtr)(void);
void setupPeriodicCallBack(voidFuncPtr pFunction, unsigned int timeMs);

bool mountStorage(FileSystemObject& drive, const char* pDescStr);
void copyLogFileToSDCard();
bool discoverExternalDevsOnI2C();
bool initializeBoardIO();
void printMemoryInfo();
void printLine() { puts("------------------------------------------"); }


/**
 * Initializes the High Level System such as IO Pins and Drivers
 */
void highLevelInitialize()
{
    /**
     * Initialize the signals used by the SD Card and Flash Memory
     */
    sd_initializeCardSignals(); // Initialize CS signal, WP and CD signals used for SD Card
    flash_InitializeSignals();  // Initialize CS signal

    /**
     * Initialize the Peripherals used in the system
     * RTC  : Used by FATFS
     * I2C2 : Used by LED Display, Acceleration Sensor, Temperature Sensor
     * ADC0 : Used by Light Sensor
     * SPI1 : Used by SD Card & External SPI Flash Memory
     */
    rtc_initialize();
    I2C2::getInstance().init(200);
    adc0_initialize();
    spi1_Init();

    /**
     * Setup Timer0 so that delay_ms(us) functions will work.
     * vConfigureTimerForRunTimeStats() is used by FreeRTOS run time statistics.
     * If FreeRTOS is used, timer will be setup anyway.
     * If FreeRTOS is not used, call it here to delay_ms(us) functions will work.
     */
    vConfigureTimerForRunTimeStats();

    /**
     * Intentional delay here because this gives us some time to
     * close COM Port at Hyperload and Open it at Hercules
     */
    delay_ms(2000);

    /**
     * Install 10ms periodic callback outside of FreeRTOS since we want to support
     * FATFS even if FreeRTOS is not running.
     *
     * Initialize the SPI Mutex that the DISK IO Layer will use (if FreeRTOS is running),
     * and then try to mount both Flash Storage and SD Card Storage and print their info.
     */
    setupPeriodicCallBack(sd_timerproc, 10);
    diskio_initializeSPIMutex((xSemaphoreHandle*) (&getHandles()->Sem.spi) );

    /**
     * If Flash is not mounted, it is probably a new board and the flash is not
     * formatted so format it, alert the user, and try to re-mount it
     */
    bool flashMounted = mountStorage(Storage::getFlashDrive(), " Flash ");
    if(!flashMounted)
    {
        printf("FLASH not formatted, formatting now ... ");
        printf("%s\n", FR_OK == Storage::getFlashDrive().format() ? "Done" : "Error");
        flashMounted = mountStorage(Storage::getFlashDrive(), " Flash ");
    }

    /**
     * If SD Card is present, try to copy log file from Flash to SD Card
     */
    bool sdMounted = mountStorage(Storage::getSDDrive(),    "SD Card");
    if(flashMounted && sdMounted)
    {
        copyLogFileToSDCard();
    }

    /**
     * SD Card specifications are 24Mhz maximum
     */
    spi1_SetMaxClockMhz(24);
    printLine();

    /**
     * Print discovered I2C devices.
     * But not really useful if no external I2C device attached.
     */
    if(discoverExternalDevsOnI2C()) {
        printLine();
    }

    /**
     * Print memory used so far.
     * This will show memory usage before main() starts.
     */
    printMemoryInfo();
    printLine();

    /**
     * Initialize all sensors of this board.
     */
    if(false == initializeBoardIO()) {
        puts("ERROR: One of Board IOs failed to initialize!");
    }

    // Display CPU speed in Mhz on LED display
    LD.setNumber(getCpuClock()/(1000*1000));
}

bool mountStorage(FileSystemObject& drive, const char* pDescStr)
{
    unsigned int totalKb = 0;
    unsigned int availKb = 0;
    bool mounted = (drive.mount() &&  FR_OK == drive.getDriveInfo(&totalKb, &availKb));

    if(mounted)
    {
        if(totalKb < 1000)
            printf("%s: OK -- Capacity %-5uKB, Available: %-5uKB\n", pDescStr, totalKb, availKb);
        else
            printf("%s: OK -- Capacity %-5uMB, Available: %-5uMB\n", pDescStr, totalKb/1024, availKb/1024);
    }
    else {
        printf("%s: Failed to mount storage!\n", pDescStr);
    }

    return mounted;
}

void copyLogFileToSDCard()
{
    // Open destination file with same filename but store into drive '1' for SD Card
    char dstFileName[] = LOGGER_FILE_NAME;
    dstFileName[0] = '1';

    printf("Moving %s to SD Card ... ", (char*)LOGGER_FILE_NAME+2);

    // Copy log file from Flash to SD Card:
    FRESULT copyStatus = Storage::copy(LOGGER_FILE_NAME, dstFileName);

    // Delete logger file from FLASH
    if(FR_OK == copyStatus) {
        f_unlink(LOGGER_FILE_NAME);
    }

    printf("%s\n", (FR_OK == copyStatus) ? "Done!" : (FR_NO_FILE == copyStatus) ? "Not found" : "ERROR!");
}

bool discoverExternalDevsOnI2C()
{
    bool found = false;

    for(unsigned int id = 2; id <= 254; id += 2)
    {
        if(I2C2::getInstance().isDevicePresent(id))
        {
            if(id != Address_AccelerationSensor &&
               id != Address_LED_Display &&
               id != Address_TemperatureSensor)
            {
                printf("Found I2C Device: 0x%02X\n", id);
                found = true;
            }
        }
    }

    return found;
}

bool initializeBoardIO()
{
    bool success = true;

    if(!AS.init()) success = false;
    if(!IS.init()) success = false;
    if(!LS.init()) success = false;
    if(!TS.init()) success = false;

    if(!LD.init()) success = false;
    if(!LE.init()) success = false;
    if(!SW.init()) success = false;

    return success;
}

static voidFuncPtr RIT_TIMER_CALLBACK = 0;
extern "C"
{
    void RIT_IRQHandler()
    {
        if(0 != RIT_TIMER_CALLBACK) {
            RIT_TIMER_CALLBACK();
        }
        // Clear Interrupt Flag
        LPC_RIT->RICTRL |= 1;
    }
}
void setupPeriodicCallBack(voidFuncPtr pFunction, unsigned int timeMs)
{
    // Divide by zero guard
    if(0 == timeMs) {
        timeMs = 1;
    }

    // Power up first otherwise writing to RIT will give us Hard Fault
    LPC_SC->PCONP |= (1 << 16);

    // Enable CLK/1 to simplify RICOMPVAL calculation below
    LPC_SC->PCLKSEL1 &= ~(3 << 26);
    LPC_SC->PCLKSEL1 |=  (1 << 26);

    LPC_RIT->RICOUNTER = 0;
    LPC_RIT->RIMASK = 0;
    LPC_RIT->RICOMPVAL = getCpuClock() / (1000 / timeMs);

    // Clear timer upon match, and enable timer
    LPC_RIT->RICTRL = (1 << 1) | (1 << 3);

    // Enable System Interrupt and connect the callback
    RIT_TIMER_CALLBACK = pFunction;
    NVIC_EnableIRQ(RIT_IRQn);
}
