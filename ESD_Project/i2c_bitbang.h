/* -----------------------------------------------------------------------------
 * Rishab Shah
 * ECEN 5613 - Spring 2021 - Prof. McClure
 * University of Colorado Boulder
 * Revised 19/03/2021
 * references:https://aticleworld.com/interfacing-eeprom-using-i2c/
 * https://8051projects.net/wiki/I2C_Implementation_on_8051#Implementing_I2C_in_C
 The function names and implementation sequence is slightly modified after
 analysing the same from the datasheet.

 The main base logic required to develop the code was taken from her for I2C.
 *  ----------------------------------------------------------------------------
 * This files handles all the functions declaration related to I2c.
   ---------------------------------------------------------------------------*/
/* ---------------------------------------------------------------------------*/
//          INCLUDES & DEFINES
/* ---------------------------------------------------------------------------*/
#ifndef I2CFILES_H
#define I2CFILES_H

#include <stdint.h>
#include <stdio.h>
#include "msp.h"

#include "i2c.h"

#define HIGH            1
#define LOW             0


#define WRITE           0
#define READ            1

#define I2C_DELAY       2

void SendNACK(void);
void SendACK();
void restart_bit();
unsigned char i2c_read_operation();
uint8_t i2c_write_operation_wakeup(uint8_t Txdata);
void mydelay(uint8_t data);
void bus_no_busy_state();
void stop_bit();
void start_bit();
void PollAck();

uint8_t eebytew(uint8_t blocknumber, uint8_t addr,uint8_t databyte);
uint8_t eebyter(uint8_t blocknumber,uint8_t addr,uint8_t blocknumberread);
void reset_eeprom();
void init_i2c_bitbang();

#endif