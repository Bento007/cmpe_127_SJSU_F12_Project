/**
 * @file io_functions.h
 * @brief	This file provides:
 *              - API to connect stdio printf/scanf to your I/O functions
 *              - Reduced printf that uses less code and RAM space
 * 			When USE_REDUCED_PRINTF is set to 1 @ sysConfig.h, this version of
 * 			printf() takes over stdio version of printf.  The only big compromise
 * 			is the lack of floating point version of printf()
 *
 * Version: 06192012    Initial
 */

#ifndef REDUCED_PRINTF_H__
#define REDUCED_PRINTF_H__
#ifdef __cplusplus
extern "C" {
#endif

/// Function pointer of a function returning a char and taking a char as parameter
typedef char(*ioFuncPtr)(char);


/**
 * Sets the function used to output a char by printf() or stdio output functions
 * @param func	The function pointer to use to output a char
 */
void stdio_SetOutputCharFunction(ioFuncPtr func);

/**
 * Sets the function used to input a char by scanf() or stdio input functions
 * @param func	The function pointer to use to get a char
 */
void stdio_SetInputCharFunction(ioFuncPtr func);

/**
 * Prints a message when critical system fault occurs
 * This sets UART0 polling version as output device and prints one char at a time using polling
 */
void printMessageUponCriticalSystemError(const char* pMsg);


#ifdef __cplusplus
}
#endif
#endif /* REDUCED_PRINTF_H__ */
