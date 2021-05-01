/******************************************************************************
* @file: uart.h
*
* @brief: This files consists of the function prototypes used in the uart.c file
* @ Used to implement serial communication over UART
* This file is required to send data over UART to the PC terminal
* @author: Rishab Shah
* @date:  20-Apr-2021
* @reference: William Goh - msp432p401x_euscia0_uart_01
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
Global variables
*******************************************************************************/
extern CB_t cbfifo_tx;
extern CB_t cbfifo_rx;
/*******************************************************************************
Function prototype
*******************************************************************************/
void EUSCIA0_IRQHandler(void);
void clock_init();
void config_uart();

#endif /* UART_H_ */
