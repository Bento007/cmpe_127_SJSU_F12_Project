#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include "memory.h"


void *operator new(size_t size)
{
    return malloc(size);
}

void *operator new[](size_t size)
{
    return malloc(size);
}

void operator delete(void *p)
{
    free(p);
}

void operator delete[](void *p)
{
    free(p);
}

extern "C"
{
MemoryInfoType getMemoryInfo()
{
    MemoryInfoType meminfo;
    const unsigned int memSize = 32 * 1024;
    const unsigned int memStart = 0x10000000;

    // This is defined by linker script (loader.ld)
    extern unsigned int _pvHeapStart;
    const unsigned int globalMem = (unsigned int) &_pvHeapStart - memStart;

    /*
    printf("Available AHG SRAM of %u KB @ 0x%08X\n", memSize/1024, 0x2007c000);
    printf("Global Memory Used    : %u\n", usedMem);
    printf("System Heap @ Startup : %u\n", (memSize - usedMem));
    */

    // Only print malloc() info if it has been used (arena is > 0)
    struct mallinfo info = mallinfo();
    /*
    if(info.arena > 0)
    {
        printf("Heap Obtained by malloc  : %5i bytes\n", info.arena);
        printf("Heap Granted by malloc   : %5i bytes\n", info.uordblks);
        printf("Heap available by malloc : %5i bytes\n", info.fordblks);
    }
    */

    meminfo.globalUsed = globalMem;
    meminfo.heapAvailable = info.fordblks;
    meminfo.heapUsed = info.uordblks;
    meminfo.systemAvailable = (memSize - meminfo.globalUsed - meminfo.heapUsed - meminfo.heapAvailable);

    return meminfo;
}
}

extern "C" int __aeabi_atexit(void *object,
		void (*destructor)(void *),
		void *dso_handle)
{
	return 0;
}


#ifndef CPP_USE_CPPLIBRARY_TERMINATE_HANDLER
/******************************************************************
 * __verbose_terminate_handler()
 *
 * This is the function that is called when an uncaught C++
 * exception is encountered. The default version within the C++
 * library prints the name of the uncaught exception, but to do so
 * it must demangle its name - which causes a large amount of code
 * to be pulled in. The below minimal implementation can reduce
 * code size noticeably. Note that this function should not return.
 ******************************************************************/
namespace __gnu_cxx {
void __verbose_terminate_handler()
{
  while(1);
}
}
#endif
