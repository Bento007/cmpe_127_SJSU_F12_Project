#include "UART_Base.hpp"
#include "LPC17xx.h"

bool UART_Base::getChar(char* pInputChar, unsigned int timeout)
{
    return xQueueReceive(mRxQueue, pInputChar, timeout);
}

bool UART_Base::putChar(char out, unsigned int timeout)
{
    // If queue is full even after the timeout, return with error
    if (!xQueueSend(mTxQueue, &out, timeout))
    {
        return false;
    }

    portENTER_CRITICAL();
    {
        long unUsed = 0;
        if (!mIntrExpected && xQueueReceiveFromISR(mTxQueue, &out, &unUsed))
        {
            mIntrExpected = true;
            mpUARTRegBase->THR = out;
        }
    }
    portEXIT_CRITICAL();

    return true;
}

void UART_Base::handleInterrupt()
{
    /**
     * Bit Masks of IIR register Bits 3:1 that contain interrupt reason.
     * Bits are shifted left because reasonForInterrupt contains Bits 3:0
     */
    const unsigned int reasonForInterrupt = (mpUARTRegBase->IIR & 0xF);
    const unsigned char transmitterEmpty = (1 << 1);
    const unsigned char dataAvailable    = (2 << 1);
    const unsigned char dataTimeout      = (6 << 1);

    long higherPriorityTaskWoken = 0;
    char c = 0;

    /**
     * If multiple sources of interrupt arise, let this interrupt exit, and re-enter
     * for the new source of interrupt.
     */
    switch (reasonForInterrupt)
    {
        case transmitterEmpty:
        {
            mIntrExpected = false;
            /**
             * When THRE (Transmit Holding Register Empty) interrupt occurs,
             * we can send as many bytes as the hardware FIFO supports (16)
             */
            const unsigned char hwTxFifoSize = 16;
            for(unsigned char i=0;
                    i < hwTxFifoSize && !xQueueIsQueueEmptyFromISR(mTxQueue);
                    i++)
            {
                if (xQueueReceiveFromISR(mTxQueue, &c, &higherPriorityTaskWoken))
                {
                    mIntrExpected = true;
                    mpUARTRegBase->THR = c;
                }
            }
            break;
        }

        case dataAvailable:
        case dataTimeout:
        {
            /**
             * While receive Hardware FIFO not empty, keep queuing the data.
             * Even if xQueueSendFromISR() Fails (Queue is full), we still need to
             * read RBR register otherwise interrupt will not clear
             */
            while (0 != (mpUARTRegBase->LSR & (1 << 0)))
            {
                c = mpUARTRegBase->RBR;
                xQueueSendFromISR(mRxQueue, &c, &higherPriorityTaskWoken);
            }
            break;
        }

        default:
            break;
    }

    if (higherPriorityTaskWoken)
    {
        vPortYieldFromISR();
    }
}

///////////////
// Protected //
///////////////
UART_Base::UART_Base(unsigned int* pUARTBaseAddr) :
        mpUARTRegBase((LPC_UART_TypeDef*) pUARTBaseAddr),
        mIntrExpected(false)
{

}

bool UART_Base::init(unsigned int pclk, unsigned int baudRate,
                        int rxQSize, int txQSize)
{
    // Configure UART Hardware: Baud rate, FIFOs etc.
    if (LPC_UART0_BASE == (unsigned int) mpUARTRegBase)
    {
        LPC_SC->PCONP |= (1 << 3); // Enable UART0
        NVIC_EnableIRQ(UART0_IRQn);
    }
    /*
     else if(LPC_UART1_BASE == (unsigned int)mpUARTRegBase)
     {
     LPC_SC->PCONP |= (1 << 4); // Enable UART1
     NVIC_EnableIRQ(UART1_IRQn);
     }
     */
    else if (LPC_UART2_BASE == (unsigned int) mpUARTRegBase)
    {
        LPC_SC->PCONP |= (1 << 24); // Enable UART2
        NVIC_EnableIRQ(UART2_IRQn);
    }
    else if (LPC_UART3_BASE == (unsigned int) mpUARTRegBase)
    {
        LPC_SC->PCONP |= (1 << 25); // Enable UART3
        NVIC_EnableIRQ(UART3_IRQn);
    }
    else
    {
        return false;
    }

    // Enable & Reset FIFOs and set 4 char timeout for Rx
    mpUARTRegBase->FCR = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 6);

    mpUARTRegBase->LCR = (1 << 7); // Enable DLAB to access DLM, DLL, and IER
    {
        mpUARTRegBase->DLM = 0;
        mpUARTRegBase->DLL = pclk / (16 * baudRate) + 0.5;
    }
    mpUARTRegBase->LCR = 3; // Disable DLAB and set 8bit per char

    // Set minimum queue size
    if (rxQSize < 9) rxQSize = 8;
    if (txQSize < 9) txQSize = 8;

    // Create the receive and transmit queues
    mRxQueue = xQueueCreate(rxQSize, sizeof(char));
    mTxQueue = xQueueCreate(txQSize, sizeof(char));

    // Enable Rx/Tx Interrupts:
    mpUARTRegBase->IER = (1 << 0) | (1 << 1); // B0:Rx, B1: Tx

    return (0 != mRxQueue && 0 != mTxQueue);
}
