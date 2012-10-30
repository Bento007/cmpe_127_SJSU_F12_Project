/**
 * @file utilities.h
 * @ingroup Utilities
 */
#ifndef UTILITIES_H__
#define UTILITIES_H__
#ifdef __cplusplus
extern "C" {
#endif



/**
 * Delays in microseconds
 * @param delayMicroSec The delay in microseconds
 */
void delay_us(unsigned int delayMicroSec);

/**
 * Delays in milliseconds
 * @param delayMilliSec The delay in milliseconds.
 */
void delay_ms(unsigned int delayMilliSec);

/**
 * Prints memory information to stdio
 */
void printMemoryInfo();

/**
 * Macro that can be used to print the timing/performance of a block
 * Example:
 * @code
 *      PRINT_EXECUTION_SPEED()
 *      {
 *          // ...
 *      }
 *      // At the end, the time taken between this block will be printed
 * @endcode
 */
#define PRINT_EXECUTION_SPEED() for(unsigned int __time=xTaskGetTickCount(); __time!=0; \
                                    printf("   Finished in %u ms\n", (unsigned int)xTaskGetTickCount()-__time),__time=0)



#ifdef __cplusplus
}
#endif
#endif /* UTILITIES_H__ */
