/******************************************************************************
* @file: gpio.h
*
* @brief: This files consists of the function definitions used in the gpio.c file
* This contains the logic to initialise the heart beat led and fan configuration through PWM le
* @date:  20-Apr-2021
* @reference: William Goh - msp432p401x_ta0_01
*******************************************************************************/
#ifndef GPIO_H_
#define GPIO_H_
/*******************************************************************************
Header files
*******************************************************************************/
#include "msp.h"
#include <stdint.h>
#include <stdio.h>
/*******************************************************************************
Macros
*******************************************************************************/
#define TURN_FAN_ON                              P2->OUT |= BIT2
#define TURN_FAN_OFF                             P2->OUT &= ~BIT2
/*******************************************************************************
Function Prototype
*******************************************************************************/
void gpio_init();

#endif /* GPIO_H_ */
