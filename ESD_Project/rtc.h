/******************************************************************************
* @file: rtc.h
*
* @brief: This files consists of the function headers used in the switch.c file
* This file is used to provide the RTC timestamp on the data which is sent
* over UART to the PC
* @author: Rishab Shah
* @date:  18-Apr-2021
* @reference: https://microdigisoft.com/single.php?id=180
*******************************************************************************/
#ifndef RTC_H_
#define RTC_H_
/*******************************************************************************
Header files
*******************************************************************************/
#include <stdio.h>
#include "msp.h"
/*******************************************************************************
Function declarations
*******************************************************************************/
void rtc_init();
void calculate_rtc_time(char* timestamp);

#endif /* RTC_H_ */
