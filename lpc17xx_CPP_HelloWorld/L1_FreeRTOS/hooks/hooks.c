#include "hooks.h"
#include <io_functions.h> // printMessageUponCriticalSystemError()
#include "core_cm3.h"     // __WFI();



void vApplicationIdleHook(void)
{
	// THIS FUNCTION MUST NOT BLOCK
	// Put CPU to IDLE here. RTOS will wake up CPU from OS timer interrupt.
	__WFI(); // Wait for Event: Puts the CPU in low powered mode
}

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
	printMessageUponCriticalSystemError("HALTING SYSTEM: Stack overflow by task: ");
	printMessageUponCriticalSystemError((char*)pcTaskName);
	printMessageUponCriticalSystemError("\nTry increasing stack memory of this task.\n");
	while(1);
}

#if configUSE_TICK_HOOK
void vApplicationTickHook( void )
{
	// This function is called at every OS Tick
	// DO NOT PUT A LOT OF CODE HERE.  KEEP IT SHORT AND SIMPLE (if you really need it)
}
#endif

#if( configUSE_MALLOC_FAILED_HOOK == 1 )
void vApplicationMallocFailedHook( void )
{
    printMessageUponCriticalSystemError("HALTING SYSTEM: Your system ran out of memory (RAM)!\n");
	while(1);
}
#endif

