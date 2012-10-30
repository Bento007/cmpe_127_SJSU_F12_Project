/**
 * @file rtc.h
 * @ingroup Drivers
 * @brief   This file provides access to the System's Real-time Clock that maintains
 *          time even through power-loss granted that RTC Backup battery is installed.
 */
#ifndef RTC_H
#define RTC_H
#ifdef __cplusplus
extern "C" {
#endif



/**
 * This struct aligns with the LPC Hardware's consolidated RTC registers.
 * The un-named chars are just for padding according to LPC CTIME registers.
 */
typedef struct {
    /*    - Data-           -Padding-  */
    unsigned sec  : 6;      unsigned : 2;
    unsigned min  : 6;      unsigned : 2;
    unsigned hour : 5;      unsigned : 3;
    unsigned dow  : 3;      unsigned : 5;

    unsigned day  : 5;      unsigned : 3;
    unsigned month: 4;      unsigned : 4;
    unsigned year :12;      unsigned : 4;
} RTC ;


void rtc_initialize (void);     /**< Initialize RTC */
RTC  rtc_gettime ();            /**< Get time */
void rtc_settime (const RTC*);  /**< Set time */



#ifdef __cplusplus
}
#endif
#endif
