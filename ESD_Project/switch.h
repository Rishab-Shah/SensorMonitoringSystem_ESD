/******************************************************************************
* @file: switch.h
*
* @brief: This files consists of the function prototypes for the function
* definition  in switch.c file
* This file is used to provide control the polling rate at which the data from
* the sensor will be read.
* @author: Rishab Shah
* @date:  13-Apr-2021
* @reference: William Goh - msp432p401x_p1_03
*******************************************************************************/
#ifndef SWITCH_H_
#define SWITCH_H_
/*******************************************************************************
Header files
*******************************************************************************/
#include "stdio.h"
#include "msp.h"
/*******************************************************************************
Macros
*******************************************************************************/
#define T_RH_BASE_POLL_FREQ                           (1000)
/*******************************************************************************
Function declarations
*******************************************************************************/
void switch_init();
void PORT1_IRQHandler(void);


#endif /* SWITCH_H_ */
