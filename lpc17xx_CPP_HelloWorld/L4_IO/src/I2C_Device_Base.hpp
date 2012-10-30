/**
 * @file I2C_Device_Base.hpp
 */
#ifndef I2C_DEVICE_BASE_HPP_
#define I2C_DEVICE_BASE_HPP_

#include "I2C2.hpp"



/**
 * I2C Device Base Class
 * This class can be inherited by an I2C Device to be able to read and write registers
 * more easily as this class will puts an abstraction layer on I2C and provides simple
 * functionality to read and write registers over I2C Bus.
 *
 * @ingroup BoardIO
 */
class I2C_Device_Base
{
protected:
    /// Constructor of this base class that takes addr as a parameter
    I2C_Device_Base(unsigned char addr) : mI2C(I2C2::getInstance()), mOurAddr(addr)
    {
    }

    /// @returns the register content of this device
    inline unsigned char readReg(unsigned char reg)
    {
        return mI2C.readReg(mOurAddr, reg);
    }

    /// Writes a register of this device
    inline void writeReg(unsigned char reg, unsigned char data)
    {
        mI2C.writeReg(mOurAddr, reg, data);
    }

    /// @returns true if the device responds to its address
    inline bool checkDeviceResponse()
    {
        return mI2C.isDevicePresent(mOurAddr);
    }

    /**
     * Reads 16-bit register from reg and reg+1 granted that reg has MSB
     */
    unsigned short get16BitRegister(unsigned char reg)
    {
        char buff[2] = {0};
        mI2C.readRegisters(mOurAddr, reg, &buff[0], sizeof(buff));

        const unsigned short MSB = buff[0];
        const unsigned short LSB = buff[1];
        return ((MSB << 8) | (LSB & 0xFF));
    }

private:
    I2C_Base& mI2C; /// Instance of I2C Bus used for communication
    const unsigned char mOurAddr; ///< I2C Address of this device
};

#endif /* I2C_DEVICE_BASE_HPP_ */
