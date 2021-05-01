/******************************************************************************
* @file: i2c_bitbang.h
*
* @brief: This files consists of the function definitions used in the i2c_bitbang.c file
* This file was incorporated to include the bit banging technique for
* AM2320 sensor for the wakeup sequence correction
* @date:  25-Apr-2021
*******************************************************************************/
#ifndef I2CFILES_H
#define I2CFILES_H
/*******************************************************************************
Header files
*******************************************************************************/
#include <am2320_i2c.h>
#include <stdint.h>
#include <stdio.h>
#include "msp.h"
/*******************************************************************************
Macros
*******************************************************************************/
#define WRITE                           (0)
#define READ                            (1)
#define I2C_DELAY                       (2)
/*******************************************************************************
Function Prototype
*******************************************************************************/
void send_nack(void);
void SendACK();
void restart_bit();
unsigned char i2c_read_operation();
void mydelay(uint8_t data);
void bus_no_busy_state();
void stop_bit();
void start_bit();
void poll_ack();
void am2320_i2c_write_operation_wakeup(uint8_t Txdata);

#endif
