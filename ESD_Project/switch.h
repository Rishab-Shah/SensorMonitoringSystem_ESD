/******************************************************************************
* @file:switch.h
*
* @brief: This files consists of the function headers used in the switch.c file
*
* @author: Rishab Shah
* @date:  12-Mar-2021
*******************************************************************************/
#ifndef SWITCH_H_
#define SWITCH_H_
/*******************************************************************************
Header files
*******************************************************************************/
#include "stdio.h"
#include "msp.h"
#include "macros.h"
/*******************************************************************************
Function declarations
*******************************************************************************/
void led_port_init();
void switch_init();
void switch_interrupt_init();
void PORT1_IRQHandler(void);


#endif /* SWITCH_H_ */
