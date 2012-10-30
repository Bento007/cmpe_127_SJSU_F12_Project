/**
 * @file uart0_min.h
 * @brief This file provides minimal UART0 implementation
 * 			The getchar and putchar functions provide polling version of data I/O
 */
#ifndef UART0_MIN_H_
#define UART0_MIN_H_
#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initializes UART0 of LPC17xx
 * @param baudRate			The baud rate to set
 * @param peripheralClock 	The peripheral clock of the system which dictates the baud rate dividers
 */
void uart0Init(unsigned int baudRate, unsigned int peripheralClock);

/**
 * @param unused Unused parameter to comply with the printfFuncPtr @see printfFuncPtr
 * @returns character received over UART0
 */
char uart0Getchar(char unused);

/**
 * @param out	The character to output over UART0
 * @returns		Zero to comply with the printfFuncPtr @see printfFuncPtr
 */
char uart0Putchar(char out);

/**
 * outputs a string using uart0Putchar()
 * @param pStr	The pointer to null terminated string
 */
void uart0Puts(const char* pStr);





#ifdef __cplusplus
}
#endif
#endif /* UART0_MIN_H_ */
