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
int Date,Month,Year,Hours,Min,Sec; // Declare Calender Variables to Read RTC
/*******************************************************************************
Function definition
*******************************************************************************/
/*******************************************************************************
* @Function switch_init
* @Description: init switch for input pullup (P1.1 and P1.4)
* @input param : None
* @return: None
*******************************************************************************/
static uint8_t BcdToDec(uint8_t bcd);

static uint8_t BcdToDec(uint8_t bcd)
{
   return( ((bcd >> 4) * 10) + (bcd&0xF) );
}

void rtc_init()
{
    RTCCTL0_H = RTCKEY_H;
    RTCCTL0_L |= RTCTEVIE;
    RTCCTL0_L &= ~(RTCTEVIFG);
    RTCCTL13 = RTCBCD | RTCHOLD ;

    //Set RTC
    RTC_C->YEAR = 0x2021;                   // Year = 0x2021
    RTC_C->DATE = (0x11 << RTC_C_DATE_MON_OFS) | // Month = 0x11 = November
         (0x25 | RTC_C_DATE_DAY_OFS);    // Day = 0x25 = 25th
    RTC_C->TIM1 = (0x01 << RTC_C_TIM1_DOW_OFS) | // Day of week = 0x01 = Monday
         (0x10 << RTC_C_TIM1_HOUR_OFS);  // Hour = 0x10
    RTC_C->TIM0 = (0x32 << RTC_C_TIM0_MIN_OFS) | // Minute = 0x32
         (0x45 << RTC_C_TIM0_SEC_OFS);   // Seconds = 0x45

    //Star RTC In Calender Mode
    RTCCTL1 &= ~(RTCHOLD);

    //Lock the RTC Register after initialization
    RTCCTL0_H= 0;
}

void calculate_rtc_time()
{
    //Time Data conversion (BCD to Decimal)
    Date=BcdToDec( RTCDAY);
    Month=BcdToDec(RTCMON);
    Year=BcdToDec( RTCYEAR);
    //Time Data conversion (BCD to Decimal)
    Hours=BcdToDec(RTCHOUR);
    Min=BcdToDec(RTCMIN);
    Sec=BcdToDec(RTCSEC);

    printf("%d %d %d %d %d %d\n",Date,Month,Year,Hours,Min,Sec);
}
/* EOF */
