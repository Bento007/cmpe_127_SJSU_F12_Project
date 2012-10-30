#include "LPC17xx.h"
#include "sysConfig.h"

void uart0Init(unsigned int baudRate, unsigned int peripheralClock)
{
	LPC_SC->PCONP |= (1 << 3);			// Enable power to UART0
	LPC_PINCON->PINSEL0 &= ~(0xF << 4); // Clear values
	LPC_PINCON->PINSEL0 |= (0x5 << 4);  // Set values for UART0 Rx/Tx

	LPC_UART0->LCR = (1 << 7);	// Enable DLAB
	LPC_UART0->DLM = 0;
	LPC_UART0->DLL = (short)(peripheralClock / (16 * baudRate) + 0.5);
	LPC_UART0->LCR = 3;			// 8-bit data
}

char uart0Putchar(char out)
{
	//while(! (LPC_UART0->LSR & (1 << 6)));
	LPC_UART0->THR = out;
    while(! (LPC_UART0->LSR & (1 << 6)));
	return 1;
}

void uart0Puts(const char* pStr)
{
	char* p = (char*) pStr;
	while(*p)
	{
		uart0Putchar(*p);
		p++;
	}
	uart0Putchar('\n');
}

char uart0Getchar(char notused)
{
	while(! (LPC_UART0->LSR & (1 << 0)))
	{
		;
	}
	return LPC_UART0->RBR;
}
