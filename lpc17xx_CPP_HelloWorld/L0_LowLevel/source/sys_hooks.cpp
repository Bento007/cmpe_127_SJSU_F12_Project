/**
 * This file hooks up C/C++ functions that require our own implementation
 * This is also where UART0 output can be tied to C/C++ printf/scanf
 */

extern "C"
{
	void _init() { }
	void _exit() { }
	void _getpid() { }
	void _kill() { }

	void _lseek() { }
	void _isatty() { }
	void _fstat() { }
	void _close() { }

	#include <stddef.h>
	/**
	 * malloc() and other memory allocations functions go here to get memory from heap.
	 * This is used by printf and other C/C++ functions.
	 * _pvHeapStart() is defined by the compiler after used RAM space.
	 *
	 * @warning There is no check for heap memory going beyond addressable RAM region
	 */
	void * _sbrk(size_t nbytes)
	{
		static char *heap_ptr = 0;
		char  *base;		/*  errno should be set to  ENOMEM on error	*/

		if (!heap_ptr) {	/*  Initialize if first time through.		*/
			extern char _pvHeapStart[];
			heap_ptr = _pvHeapStart;
		}

		base = heap_ptr;	/*  Point to end of heap.		*/
		heap_ptr += nbytes;	/*  Increase heap.				*/

		/**
		 * TODO 32Kb is also available at 0x2007c000
		 * When this heap runs close to the end, we can release
		 * memory from this 2nd SRAM region
		 */
		return base;		/*  Return pointer to start of new heap area.	*/
	}
}
