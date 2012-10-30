/**
 * @file  uart0.hpp
 * @brief UART0 Interrupt driven IO driver
 */
#ifndef UART0_HPP_
#define UART0_HPP_

#include "UART_Base.hpp"          // Base class
#include "singletonTemplate.hpp"  // Singleton Template


/**
 * UART0 Interrupt Driven Driver
 * This simply uses UART_Base, and forwards the UART0 IRQ to UART_Base::handleInterrupt()
 * This enforces Singleton class, otherwise it's a thin wrapper around UART_Base class.
 *
 * @ingroup Drivers
 */
class UART0 : public UART_Base, public SingletonTemplate<UART0>
{
    public:
        /**
         * Initializes UART0 at the given @param baudRate
         * @param rxQSize   The size of the receive queue  (optional, defaults to 32)
         * @param txQSize   The size of the transmit queue (optional, defaults to 64)
         */
        bool init(unsigned int baudRate, int rxQSize=32, int txQSize=64);

        /**
         * @{ \name Static functions to use for printf/scanf redirection.  @see
         *          stdio_SetInputCharFunction() and stdio_SetOutptCharFunction()
         */
        static char getcharIntrDriven(char unused);
        static char putcharIntrDriven(char thechar);
        /** @} */

    private:
        UART0();  ///< Private constructor of this Singleton class
        friend class SingletonTemplate<UART0>;  ///< Friend class used for Singleton Template
};


#endif /* UART0_HPP_ */
