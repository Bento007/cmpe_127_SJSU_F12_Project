#include <stddef.h>    		// size_t
#include "io_functions.h"   // printfFuncPtr
#include "sysConfig.h" 		// #if USE_REDUCED_PRINTF
#include "uart0_min.h"


void printMessageUponCriticalSystemError(const char* pMsg)
{
	stdio_SetOutputCharFunction(uart0Putchar);
	uart0Puts(pMsg);
}




static ioFuncPtr mOutputDevFuncPtr = 0; ///< Function pointer for output function
static ioFuncPtr mInputDevFuncPtr = 0;  ///< Function pointer for input function

void stdio_SetOutputCharFunction(ioFuncPtr func)
{
	mOutputDevFuncPtr = func;
}
void stdio_SetInputCharFunction(ioFuncPtr func)
{
	mInputDevFuncPtr = func;
}

/**
 * stdio scanf and getchar at this file goes here to read a char
 */
size_t _read(int file, void *ptr, size_t len)
{
	if (0 != mInputDevFuncPtr)
	{
		char* pChar = (char*) ptr;
		//while (len--)
		{
			*pChar++ = mInputDevFuncPtr(0);
		}
	}

	return (size_t) (1);
}

/**
 * stdio printf and putchar at this file goes here to output a char
 */
size_t _write(int file, const void *ptr, size_t len)
{
	if (0 != mOutputDevFuncPtr)
	{
		char* d = (char*)ptr;
		for (int i=0; i < len; i++)
		{
			mOutputDevFuncPtr(d[i]);
		}
	}

	return len;
}





#if USE_REDUCED_PRINTF == 0

    // Full printf in use so nothing defined here

#elif USE_REDUCED_PRINTF == 1
    #include <stdarg.h>
    #include <stdio.h>

    /**
     * For this version of REDUCED PRINTF, simply hand off to stdio iprintf and siprintf
     * which should reduce code size by about 16KBytes!!
     */
    int printf(const char* fmt, ...)
    {
        int len = 0;
        va_list args;
        va_start(args, fmt);
        len = viprintf(fmt, args);
        va_end(args);
        return len;
    }
    int sprintf(char* pDst, const char* fmt, ...)
    {
        int len = 0;
        va_list args;
        va_start(args, fmt);
        len = vsiprintf(pDst, fmt, args);
        va_end(args);
        return len;
    }
#else
#include <stdarg.h>

static void printchar(char **str, int c)
{
	if (str)
	{
		**str = c;
		++(*str);
	}
	else
		_write(0, &c, 1);
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int prints(char **out, const char *string, int width, int pad)
{
	register int pc = 0, padchar = ' ';

	if (width > 0)
	{
		register int len = 0;
		register const char *ptr;
		for (ptr = string; *ptr; ++ptr)
			++len;
		if (len >= width)
			width = 0;
		else
			width -= len;
		if (pad & PAD_ZERO)
			padchar = '0';
	}
	if (!(pad & PAD_RIGHT))
	{
		for (; width > 0; --width)
		{
			printchar(out, padchar);
			++pc;
		}
	}
	for (; *string; ++string)
	{
		printchar(out, *string);
		++pc;
	}
	for (; width > 0; --width)
	{
		printchar(out, padchar);
		++pc;
	}

	return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[PRINT_BUF_LEN];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = i;

	if (i == 0)
	{
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints(out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0)
	{
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN - 1;
	*s = '\0';

	while (u)
	{
		t = u % b;
		if (t >= 10)
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= b;
	}

	if (neg)
	{
		if (width && (pad & PAD_ZERO))
		{
			printchar(out, '-');
			++pc;
			--width;
		}
		else
		{
			*--s = '-';
		}
	}

	return pc + prints(out, s, width, pad);
}

static int print(char **out, const char *format, va_list args)
{
	register int width, pad;
	register int pc = 0;
	char scr[2];

	for (; *format != 0; ++format)
	{
		if (*format == '%')
		{
			++format;
			width = pad = 0;
			if (*format == '\0')
				break;
			if (*format == '%')
				goto out;
			if (*format == '-')
			{
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0')
			{
				++format;
				pad |= PAD_ZERO;
			}
			for (; *format >= '0' && *format <= '9'; ++format)
			{
				width *= 10;
				width += *format - '0';
			}
			if (*format == 's')
			{
				register char *s = (char *) va_arg( args, int );
				pc += prints(out, s ? s : "(null)", width, pad);
				continue;
			}
			if (*format == 'd')
			{
				pc += printi(out, va_arg( args, int ), 10, 1, width, pad, 'a');
				continue;
			}
			if (*format == 'x')
			{
				pc += printi(out, va_arg( args, int ), 16, 0, width, pad, 'a');
				continue;
			}
			if (*format == 'X')
			{
				pc += printi(out, va_arg( args, int ), 16, 0, width, pad, 'A');
				continue;
			}
			if (*format == 'i' || *format == 'd')
			{
				pc += printi(out, va_arg(args,int) , 10, 0, width, pad, 'a');
				continue;
			}
			if (*format == 'u')
			{
				pc += printi(out, va_arg( args, unsigned int ), 10, 0, width, pad, 'a');
				continue;
			}
			if (*format == 'f')
			{
				/* Experimental floating point printf */
				double fnum = va_arg( args, double );
				int x = fnum;
				pc += printi(out, fnum, 10, 0, width, pad, 'a');
				x = (fnum - x) * 1000 * 1000;
				pc += printi(out, fnum, 10, 0, width, pad, 'a');

				continue;
			}
			if (*format == 'c')
			{
				/* char are converted to int then pushed on the stack */
				scr[0] = (char) va_arg( args, int );
				scr[1] = '\0';
				pc += prints(out, scr, width, pad);
				continue;
			}
		}
		else
		{
			out: printchar(out, *format);
			++pc;
		}
	}
	if (out)
		**out = '\0';

	va_end( args);
	return pc;
}

int printf(const char *format, ...)
{
	va_list args;

	va_start( args, format);
	return print(0, format, args);
}

int sprintf(char *out, const char *format, ...)
{
	va_list args;

	va_start( args, format);
	return print(&out, format, args);
}

#endif

