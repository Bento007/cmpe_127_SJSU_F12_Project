#include <stdio.h>


#include "utilities.h"
#include "LPC17xx.h"
#include "sysConfig.h" // TIMER0_US_PER_TICK
#include "memory.h"


#include "FreeRTOS.h"
#include "task.h"


/**
 * Delays in microseconds
 * @param delayMicroSec The delay in microseconds
 */
inline void delay_us(unsigned int delayMicroSec)
{
    const unsigned int ticksForThisDelay = delayMicroSec / TIMER0_US_PER_TICK;
    const unsigned int targetTick = LPC_TIM0->TC + ticksForThisDelay;

    // Take care of TC overflow case by allowing TC value to go beyond
    // targetTick, and then the next loop will handle the rest.
    if(targetTick < LPC_TIM0->TC) {
        while(LPC_TIM0->TC > targetTick) {
            ;
        }
    }

    // Wait until the TC value reaches targetTick
    while(LPC_TIM0->TC < targetTick) {
        ;
    }
}

/**
 * Delays in milliseconds
 * @param delayMilliSec The delay in milliseconds.
 */
void delay_ms(unsigned int delayMilliSec)
{
    /**
     * If FreeRTOS is running, we should sleep the calling task instead
     * of polling using the full CPU.
     */
    if(taskSCHEDULER_RUNNING == xTaskGetSchedulerState())
    {
        vTaskDelay(OS_MS(delayMilliSec));
    }
    else
    {
        delay_us(1000 * delayMilliSec);
    }
}

void printMemoryInfo()
{
    MemoryInfoType info = getMemoryInfo();
    printf("Memory Information:\n"
            "Global Used   : %5u\n"
            "Heap   Used   : %5u\n"
            "Heap Avail.   : %5u\n"
            "System Avail. : %5u\n",
            info.globalUsed, info.heapUsed, info.heapAvailable, info.systemAvailable);
}

