/******************************************************************************
* @file:switch.c
*
* @brief: This files consists of the function headers used in the switch.c file
*
* @author: Rishab Shah
* @date:  12-Mar-2021
*******************************************************************************/
/*******************************************************************************
Header files
*******************************************************************************/
#include "rtc.h"
/*******************************************************************************
Global variables
*******************************************************************************/

/*******************************************************************************
Function definition
*******************************************************************************/
/*******************************************************************************
* @Function switch_init
* @Description: init switch for input pullup (P1.1 and P1.4)
* @input param : None
* @return: None
*******************************************************************************/
static uint8_t bcd_to_dec(uint8_t bcd);


void rtc_init()
{
    RTCCTL0_H = RTCKEY_H;
    RTCCTL0_L |= RTCTEVIE;
    RTCCTL0_L &= ~(RTCTEVIFG);
    RTCCTL13 = RTCBCD | RTCHOLD ;

    //Set RTC
    RTC_C->YEAR = 0x2021;                           // Year = 0x2021
    RTC_C->DATE = (0x11 << RTC_C_DATE_MON_OFS) |    // Month = 0x04 = November
         (0x25 | RTC_C_DATE_DAY_OFS);               // Day = 0x25 = 25th
    RTC_C->TIM1 = (0x01 << RTC_C_TIM1_DOW_OFS) |    // Day of week = 0x01 = Monday
         (0x23 << RTC_C_TIM1_HOUR_OFS);             // Hour = 0x11
    RTC_C->TIM0 = (0x59 << RTC_C_TIM0_MIN_OFS) |    // Minute = 0x59
         (0x45 << RTC_C_TIM0_SEC_OFS);              // Seconds = 0x45

    //Star RTC In Calender Mode
    RTCCTL1 &= ~(RTCHOLD);

    //Lock the RTC Register after initialization
    RTCCTL0_H= 0;
}

void calculate_rtc_time(char* timestamp)
{
    /* Declare Calender Variables to Read RTC */
    int date,month,year,hour,min,sec;

    //Time Data conversion (BCD to Decimal)
    date = bcd_to_dec( RTCDAY);
    month = bcd_to_dec(RTCMON);
    year = bcd_to_dec( RTCYEAR);

    //Time Data conversion (BCD to Decimal)
    hour = bcd_to_dec(RTCHOUR);
    min = bcd_to_dec(RTCMIN);
    sec = bcd_to_dec(RTCSEC);

    /* "DD:MM:YY:HH:MM:SS::*/
    sprintf(timestamp,"%02d:%02d:%02d:%02d:%02d:%02d::",date,month,year,hour,min,sec);
}

static uint8_t bcd_to_dec(uint8_t bcd_data)
{
   return( ((bcd_data >> 4) * 10) + (bcd_data & 0xF) );
}

/* EOF */
