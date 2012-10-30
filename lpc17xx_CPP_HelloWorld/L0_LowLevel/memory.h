/**
 * @file memory.h
 * @brief API at this file can be used to print out Memory Information of the System
 *          The corresponding, source file also contains the C++ memory allocation
 *          and deallocation operators which are "new" and "delete".
 */
#ifndef MEMORY_H_
#define MEMORY_H_
#ifdef __cplusplus
extern "C" {
#endif



/**
 * This is the memory structure that is returned from getMemoryInfo()
 * Heap memory obtains pool of memory from System, so the memory pool
 * not obtained by the Heap is listed by the systemAvailable variable.
 */
typedef struct
{
    unsigned globalUsed;        ///< Global Memory allocated
    unsigned heapUsed;          ///< Memory granted by Heap (malloc, new etc.)
    unsigned heapAvailable;     ///< Memory available at Heap
    unsigned systemAvailable;   ///< Memory available to Heap
}MemoryInfoType;

/**
 * Gets System Memory information
 * The information includes Global Memory usage, and dynamic memory usage.
 * @returns MemoryInfoType structure
 */
MemoryInfoType getMemoryInfo();



#ifdef __cplusplus
}
#endif
#endif /* MEMORY_H_ */
