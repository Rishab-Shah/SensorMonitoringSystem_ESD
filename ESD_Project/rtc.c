/******************************************************************************
* @file: rtc.c
*
* @brief: This files consists of the function headers used in the rtc.h file
* This file is used to provide the RTC timestamp on the data which is sent
* over UART to the PC
* @author: Rishab Shah
* @date:  18-Apr-2021
* @reference: https://microdigisoft.com/single.php?id=180
*******************************************************************************/
/*******************************************************************************
Header files
*******************************************************************************/
#include "rtc.h"
/*******************************************************************************
Function declarations
*******************************************************************************/
static uint8_t bcd_to_dec(uint8_t bcd);
/*******************************************************************************
Function definition
*******************************************************************************/
/*******************************************************************************
* @Function rtc_init
* @Description: Initializes the RTC module
* @input param : None
* @return: None
* @reference: https://microdigisoft.com/single.php?id=180
*******************************************************************************/
void rtc_init()
{
    /* RTC_C Key Value for RTC_C write access */
    RTCCTL0_H = RTCKEY_H;

    /* Real-time clock time event interrupt enable */
    RTCCTL0_L |= RTCTEVIE;

    /* Real-time clock time event interrupt flag */
    RTCCTL0_L &= ~(RTCTEVIFG);

    /* Real-time clock BCD select */
    /* Real-time clock hold */
    RTCCTL13 = RTCBCD | RTCHOLD ;

    /* Set RTC timestamp to start with */
    // Year = 0x2021
    RTC_C->YEAR = 0x2021;

    // Month = 0x04 = April
    // Day = 0x25 = 25th
    RTC_C->DATE = (0x04 << RTC_C_DATE_MON_OFS)
         | (0x25 | RTC_C_DATE_DAY_OFS);

    // Day of week = 0x01 = Monday
    // Hour = 0x18 6 pm
    RTC_C->TIM1 = (0x01 << RTC_C_TIM1_DOW_OFS)
         | (0x18 << RTC_C_TIM1_HOUR_OFS);

    // Minute = 0x00
    // Seconds = 0x00
    RTC_C->TIM0 = (0x00 << RTC_C_TIM0_MIN_OFS)
         | (0x00 << RTC_C_TIM0_SEC_OFS);

    /* Start RTC In Calender Mode */
    RTCCTL1 &= ~(RTCHOLD);

    /* Lock the RTC Register after initialization */
    RTCCTL0_H= 0;
}

/*******************************************************************************
* @Function calculate_rtc_time
* @Description: Calculates the RTC time and generates it in the string format
* @input param : None
* @return: None
* @reference : https://microdigisoft.com/single.php?id=180
*******************************************************************************/
void calculate_rtc_time(char* timestamp)
{
    /* Calendar variables for reading RTC data */
    int date,month,year,hour,min,sec;

    /* Obtaining Day,Month,Year */
    date = bcd_to_dec(RTCDAY);
    month = bcd_to_dec(RTCMON);
    year = bcd_to_dec(RTCYEAR);

    /* Obtaining Hour,Min,Sec */
    hour = bcd_to_dec(RTCHOUR);
    min = bcd_to_dec(RTCMIN);
    sec = bcd_to_dec(RTCSEC);

    /* "DD:MM:YY:HH:MM:SS::*/
    sprintf(timestamp,"%02d:%02d:%02d:%02d:%02d:%02d::",date,month,year,hour,min,sec);
}

/*******************************************************************************
* @Function bcd_to_dec
* @Description: Converts bcd data to decimal equivalent
* @input param : None
* @return: None
* @reference: http://wiring.org.co/learning/libraries/realtimeclock.html
*******************************************************************************/
static uint8_t bcd_to_dec(uint8_t bcd_data)
{
    /* ( (val/16*10) + (val%16) ) */
   return( ((bcd_data >> 4) * 10) + (bcd_data & 0xF) );
}

/* EOF */
