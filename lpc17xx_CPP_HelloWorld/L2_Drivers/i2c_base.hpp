/**
 * @file  i2c_base.hpp
 * @brief Provides I2C Base class functionality for I2C peripherals
 */
#ifndef I2C_BASE_HPP_
#define I2C_BASE_HPP_

#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "task.h"       // xTaskGetSchedulerState()
#include "semphr.h"     // Semaphores used in I2C



/**
 * For mutli-byte I2C transfers, this macro defines the maximum buffer size
 * to perform multi byte transfers.
 */
#define I2C_FRAME_MAX_DATABYTES  4

/**
 * Define the maximum timeout for read operation (in case error occurs)
 */
#define I2C_READ_TIMEOUT_MS      100


/**
 * I2C Base class that can be used to write drivers for all I2C peripherals.
 *
 * @ingroup Drivers
 */
class I2C_Base
{
    public:
        /**
         * When the I2C interrupt occurs, this function should be called to handle
         * future action to take due to the interrupt cause.
         */
        void handleInterrupt();

        /**
         * Reads a single byte from an I2C Slave
         * @param deviceAddress     The I2C Device Address
         * @param registerAddress   The register address to read
         * @return The byte read from slave device (might be 0 if error)
         */
        char readReg(char deviceAddress, char registerAddress);

        /**
         * Writes a single byte to an I2C Slave
         * @param deviceAddress     The I2C Device Address
         * @param registerAddress   The register address to write
         * @param value             The value to write to registerAddress
         * @return 0 if successful, or one of I2C States as error condition
         */
        char writeReg(char deviceAddress, char registerAddress, char value);

        /**
         * Reads multiple bytes from an I2C device starting from the first register
         * It is assumed that like almost all I2C devices, the register address increments by 1
         * upon reading each byte.  This is usually how all I2C devices work.
         * @param deviceAddress     The device address to read data from
         * @param firstReg          The first register to read from
         * @param pData             The pointer to where to store the data
         * @param bytesToRead       The number of bytes to read
         * @returns true if the read was successful
         */
        bool readRegisters(char deviceAddress, char firstReg, char* pData, unsigned int bytesToRead);

        /**
         * This function can be used to check if an I2C device responds to its address, which
         * can therefore be used to discover all I2C hardware devices.
         * Sometimes this method is used by devices to check if they are ready for futher operations
         * such as an EEPROM or FLASH memory.
         * @param deviceAddress     The device address to check for I2C response
         * @returns true if I2C device with given address is ready
         */
        char isDevicePresent(char deviceAddress);



    protected:
        /**
         * Protected constructor that requires parent class to provide I2C
         * base register address for which to operate this I2C driver
         */
        I2C_Base(LPC_I2C_TypeDef* pI2CBaseAddr);

        /**
         * Initializes I2C Communication BUS
         * @param pclk  The peripheral clock to the I2C Bus
         * @param busRateInKhz  The speed to set for this I2C Bus
         */
        bool init(unsigned int pclk, unsigned int busRateInKhz);



    private:
        LPC_I2C_TypeDef* mpI2CRegs;    ///< Pointer to I2C memory map
        IRQn_Type        mIRQ;         ///< IRQ of this I2C
        xSemaphoreHandle mI2CMutex;    ///< I2C Mutex used when FreeRTOS is running
        xSemaphoreHandle mReadCompSig; ///< Signal that indicates read is complete

        /**
         * The status of I2C is returned from the I2C function that handles state machine
         */
        typedef enum {
            busy,
            readComplete,
            writeComplete
        } __attribute__((packed)) mI2CStateMachineStatusType;

        /**
         * This enumeration dictates read vs. write operation of an I2C transaction
         */
        typedef enum {
            i2cWrite,
            i2cRead
        } __attribute__((packed)) mI2cModeType;

        /**
         * This structure contains I2C transaction parameters
         */
        typedef struct
        {
            char slaveAddress;  ///< Slave Device Address
            char firstRegister; ///< 1st Register to Read or Write
            char error;         ///< Error if any occurred within I2C

            mI2cModeType mode;  ///< Tracks read vs. write mode

            char rwBuffer[I2C_FRAME_MAX_DATABYTES]; ///< Buffer of the I2C Read or Write
            short bytesToTransfer;                  ///< # of bytes to transfer.
            short bytePointer;                      ///< Tracks the byte number being read or written.
        }mI2CFrameType;

        /// The I2C Input Output frame that contains I2C transaction information
        mI2CFrameType mI2CIOFrame;

        /**
         * When an interrupt occurs, this handles the I2C State Machine action
         * @returns The status of I2C State Machine, which are:
         *              - Busy
         *              - Write is complete
         *              - Read  is complete
         */
        mI2CStateMachineStatusType i2cStateMachine();

        /**
         * This is the entry point for an I2C transaction
         * @param mode      Read or Write Mode
         * @param devAddr   The address of the I2C Device
         * @param regStart  The register address of I2C device to read or write
         * @param pBytes    The pointer to one or more data bytes to read or write
         * @param len       The length of the I2C transaction
         */
        void i2cKickOffTransfer(mI2cModeType mode, char devAddr, char regStart, char* pBytes, int len);

};




#endif /* I2C_BASE_HPP_ */
