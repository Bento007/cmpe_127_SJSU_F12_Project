/**
 * @file  I2C2.hpp
 * @brief I2C2 Interrupt driven IO driver
 */
#ifndef I2C2_HPP_
#define I2C2_HPP_



#include "singletonTemplate.hpp"
#include "i2c_base.hpp"



/**
 * I2C2 Singleton Driver
 * This is a thin wrapper around I2C_Base class and merely gives the base
 * address of the I2C2 memory map.
 *
 * @ingroup Drivers
 */
class I2C2 : public I2C_Base, public SingletonTemplate<I2C2>
{
    public:
        /// Initializes I2C2 at the given @param speedInKhz
        bool init(unsigned int speedInKhz);

    private:
        I2C2(); ///< Private constructor for this singleton class
        friend class SingletonTemplate<I2C2>;  ///< Friend class used for Singleton Template
};



#endif /* I2C2_HPP_ */
