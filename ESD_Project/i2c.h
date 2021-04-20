/*
 * i2c.h
 *
 *  Created on: 11-Apr-2021
 *      Author: HP
 */

#ifndef I2C_H_
#define I2C_H_

#include "msp.h"
#include <stdint.h>
#include "systimer.h"

void i2c_init();
void i2c_write_wakeup();
void i2c_write_data();

void i2c_read_data();
void clear_i2c_buffer();


void get_RH_and_temperature(unsigned int *data1, signed int *data2);
void get_CRC(unsigned int *CRC_data);
unsigned int CRC16(unsigned char *ptr, unsigned char length);

#endif /* I2C_H_ */
