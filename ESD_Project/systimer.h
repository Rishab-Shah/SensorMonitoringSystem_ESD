/******************************************************************************
* @file: systimer.h
*
* @brief: This files consists of the function prototypes and macros
*  used in the systimer.c file
* This file is used to provide the systick timer feature to carry out time based
* activities
* @author: Rishab Shah
* @date:  12-Apr-2021
*******************************************************************************/
#ifndef SYSTIMER_H_
#define SYSTIMER_H_
/*******************************************************************************
Header files
*******************************************************************************/
#include "msp.h"
#include <stdint.h>
/*******************************************************************************
Macros
*******************************************************************************/
#define MSEC_1                          1
#define MSEC_10                         10
#define MSEC_100                        100
#define MSEC_1000                       1000
#define RESOLUTION                      MSEC_1
/*******************************************************************************
Function prototype
*******************************************************************************/
void TA0_0_IRQHandler(void);
void init_timer();
void reset_timer();
int delay_msec();
void delay_usec(int n);
#endif /* SYSTIMER_H_ */
