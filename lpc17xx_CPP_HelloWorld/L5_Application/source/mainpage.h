/**
 * @file mainpage.h
 *
 * \mainpage    Official SJSU-One Sample Project Documentation
 * \section     PreSetup1   Drivers that are initialized:
 *                  - Minimal, Polling UART0 initialized @ 38400BPS
 *                  - RTC
 *                  - ADC
 *                  - SPI1
 *                  - I2C2
 *
 * \section     PreSetup2   Other items that are setup:
 *                  - PLL Setup according to desired CPU speed set @ sysConfig.h
 *                  - Flash Acceleration setup according to CPU frequency
 *                  - FreeRTOS fully setup, just need to create tasks and start FreeRTOS scheduler
 *                  - Attempt is made to mount SD Card & Flash Memory, so File IO is ready
 *
 * \section     Resources   The following resources are setup before main() is called
 *                  - System Tick Interrupt is setup for FreeRTOS
 *                  - Timer0 is enabled for FreeRTOS to collect Runtime status (CPU Usage of each task)
 *                  - Timer1 is enabled to capture and decode the IR signal
 *                  - Repetitive interrupt (RIT) of 10ms is setup to support FAT System's SD Card
 *
 *
 *
 * \section     Layer0  Layer 0 sets up the following items:
 *                  - Setup the System Memory and CPU Clock
 *                  - Initialize minimal UART0 (polling version) for standard I/O
 *                  - Make a call to highLevelInitialize() followed by call to main()
 *                  - This layer also contains the processor memory map file
 *
 * \section     Layer1  Layer 1 provides the FreeRTOS API
 *                  - Some source code is optimized and improved to provide better information.
 *
 * \section     Layer2  Layer 2 provides the System Drivers
 *                  - All of your drivers should be placed in this layer.
 *                  - Base classes for UART and I2C are present to help write drivers for additional UARTs
 *
 * \section     Layer3  Layer 3 provides Utility functions and classes
 *                  - Utility functions contain embedded tweaked String and Vector class
 *                  - Other classes include LOGGER, Command Handler, and FreeRTOS wrapper around Timers
 *
 * \section     Layer4  Layer 4 provides gateway to the SJ One Board IO that includes:
 *                  - FAT File System to access SD Card and on-board Flash Memory
 *                  - Storage class that allows to format and get information about SD and Flash Memory
 *                  - Acceleration, IR, Temperature, and Light Sensors
 *                  - LED Display, LEDs, and Switches class
 *
 * \section     Layer5  Layer 5 is where your application code should go including:
 *                  - FreeRTOS Tasks
 *                  - High level code that interacts with lower layers
 */

 /**
 * \defgroup Drivers    Drivers used in the Project
 * \defgroup BoardIO    Board Input or Output Devices
 * \defgroup Utilities  Utility classes
 */
