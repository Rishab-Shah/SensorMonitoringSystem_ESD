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

#define SDA                         BIT6
#define SCL                         BIT7
#define SEL_0                       P1->SEL0
#define SEL_1                       P1->SEL1

void i2c_init();
void i2c_write_wakeup();
void i2c_write_data(uint8_t function_code ,uint8_t start_address ,uint8_t registerlength,uint8_t length_data);

void i2c_read_data(uint8_t bytes_read);
void clear_i2c_buffer();


void get_RH_and_temperature(unsigned int *data1, signed int *data2);
void get_CRC(unsigned int *CRC_data);
unsigned int CRC16(unsigned char *ptr, unsigned char length);


#define FUNCTION_CODE       0x03
#define START_ADDRESS       0x00
#define REGISTER_LENGTH     0x04

#endif /* I2C_H_ */
