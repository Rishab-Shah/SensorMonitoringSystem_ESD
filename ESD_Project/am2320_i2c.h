/*
 * i2c.h
 *
 *  Created on: 11-Apr-2021
 *      Author: HP
 */

#ifndef AM2320_I2C_H_
#define AM2320_I2C_H_

#include "msp.h"
#include <stdint.h>
#include "systimer.h"

#define SDA                         BIT6
#define SCL                         BIT7
#define SEL_0                       P1->SEL0
#define SEL_1                       P1->SEL1

void am2320_i2c_init();
void am2320_i2c_write_wakeup();
void am2320_i2c_write_data(uint8_t function_code ,uint8_t start_address ,uint8_t registerlength,uint8_t length_data);

void am2320_i2c_read_data(uint8_t bytes_read);
void am2320_clear_i2c_buffer();


void am2320_get_RH_and_temperature(unsigned int *data1, signed int *data2);
void am2320_get_CRC(unsigned int *CRC_data);
unsigned int am2320_CRC16(unsigned char *ptr, uint8_t length);


#define FUNCTION_CODE       0x03
#define START_ADDRESS       0x00
#define REGISTER_LENGTH     0x04

#endif /* AM2320_I2C_H_ */
