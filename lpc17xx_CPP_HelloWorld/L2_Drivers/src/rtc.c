#include "rtc.h"
#include "LPC17xx.h"

void rtc_initialize (void)
{
    LPC_SC->PCONP |= (1 << 9);  // Enable RTC Power
    LPC_RTC->CCR = 1;           // Enable RTC
}

RTC rtc_gettime ()
{
    // Read the struct from LPC Memory Map
    return  *(RTC*) (&LPC_RTC->CTIME0) ;
}

void rtc_settime (const RTC *rtc)
{
	/*
	 * Update RTC registers :
	 * Note: Cannot write to consolidated registers since they are read-only
	 */
    LPC_RTC->SEC   = rtc->sec;
	LPC_RTC->MIN   = rtc->min;
	LPC_RTC->HOUR  = rtc->hour;
	LPC_RTC->DOW   = rtc->dow;
	LPC_RTC->DOM   = rtc->day;
	LPC_RTC->MONTH = rtc->month;
	LPC_RTC->YEAR  = rtc->year;
}
