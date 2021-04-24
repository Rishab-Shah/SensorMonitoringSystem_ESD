/******************************************************************************
* @file:switch.h
*
* @brief: This files consists of the function headers used in the switch.c file
*
* @author: Rishab Shah
* @date:  12-Mar-2021
*******************************************************************************/
#ifndef RTC_H_
#define RTC_H_
/*******************************************************************************
Header files
*******************************************************************************/
#include "stdio.h"
#include "msp.h"
//#include "macros.h"
/*******************************************************************************
Function declarations
*******************************************************************************/
void rtc_init();
void calculate_rtc_time(char* timestamp);

#endif /* RTC_H_ */
