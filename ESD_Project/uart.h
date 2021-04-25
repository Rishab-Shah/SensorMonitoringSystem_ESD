/******************************************************************************
* @file:uart.h
*
* @brief: This files consists of the function headers used in the uart.c file
*
* @author: Rishab Shah
* @date:  12-Mar-2021
*******************************************************************************/
#ifndef UART_H_
#define UART_H_
/*******************************************************************************
Header files
*******************************************************************************/
#include "stdio.h"
#include "msp.h"
#include "cbfifo.h"
/*******************************************************************************
Function prototype
*******************************************************************************/
extern CB_t cbfifo_tx;
extern CB_t cbfifo_rx;

void EUSCIA0_IRQHandler(void);
void clock_init();
void config_uart();


#endif /* UART_H_ */
