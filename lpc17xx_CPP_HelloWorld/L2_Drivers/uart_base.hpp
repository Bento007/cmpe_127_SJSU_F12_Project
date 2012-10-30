/**
 * @file  UART_Base.hpp
 * @brief Provides UART0 Base class functionality for UART peripherals
 */
#ifndef UART_BASE_HPP_
#define UART_BASE_HPP_

#include "LPC17xx.h"
#include "FreeRTOS.h"
#include "queue.h"

/**
 * UART Base class that can be used to write drivers for all UART peripherals.
 * Steps needed to write a UART driver:
 *  - Inherit this class
 *  - Call init() and configure PINSEL to select your UART pins
 *  - When your UART(#) hardware interrupt occurs, call handleInterrupt()
 *
 *  @warning This class doesn't work for UART1 due to different memory map.
 *  @ingroup Drivers
 */
class UART_Base
{
    public:
        /**
         * @returns a character from the UART input
         * @param   pInputChar  The pointer to input char to store received character
         * @param   timeout Optional parameter which defaults to maximum value that
         *          will allow you to wait forever for a character to be received
         * @returns true if a character was obtained within the given timeout
         */
        bool getChar(char* pInputChar, unsigned int timeout=portMAX_DELAY);

        /**
         * Outputs a char given by @param out
         * @param   timeout Optional parameter which defaults to maximum value that
         *          will allow you to wait forever for a character to be sent
         * @returns true if the output char was successfully written to Queue, or
         *          false if the output queue was full within the given timeout
         */
        bool putChar(char out, unsigned int timeout=portMAX_DELAY);

        /**
         * When the UART interrupt occurs, this function should be called to handle
         * future action to take due to the interrupt cause.
         */
        void handleInterrupt();

    protected:
        /**
         * Protected constructor that requires parent class to provide UART's
         * base register address for which to operate this UART driver
         */
        UART_Base(unsigned int* pUARTBaseAddr);

        /**
         * Initializes the UART register including Queues, baudrate and hardware.
         * Parent class should call this method before initializing Pin-Connect-Block
         * @param pclk      The system peripheral clock for this UART
         * @param baudRate  The baud rate to set
         * @param rxQSize   The receive queue size
         * @param txQSize   The transmit queue size
         * @post    Sets 8-bit mode, no parity, no flow control.
         * @warning This will not initialize the PINS, so user needs to do pin
         *          selection because LPC's same UART hardware, such as UART2
         *          is available on multiple pins.
         * @note If the txQSize is too small, functions performing printf will start to block.
         */
        bool init(unsigned int pclk, unsigned int baudRate, int rxQSize=32, int txQSize=32);

    private:
        /// Pointer to UART's memory map
        LPC_UART_TypeDef* mpUARTRegBase;

        xQueueHandle mRxQueue; ///< Queue for UARTs receive buffer
        xQueueHandle mTxQueue; ///< Queue for UARTs transmit buffer
        bool mIntrExpected;    ///< Tracks if THRE interrupt is expected
};




#endif /* UART_BASE_HPP_ */
