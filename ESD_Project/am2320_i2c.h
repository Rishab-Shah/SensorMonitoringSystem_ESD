/******************************************************************************
* @file: i2c_bitbang.h
*
* @brief: This files consists of the function definitions used in the i2c_bitbang.c file
* This file was incorporated to include the bit banging technique for
* AM2320 sensor for the wakeup sequence correction
* @date:  25-Apr-2021
* @reference: William Goh - msp432p401x_ta0_01
*******************************************************************************/
#ifndef AM2320_I2C_H_
#define AM2320_I2C_H_
/*******************************************************************************
Header files
*******************************************************************************/
#include "msp.h"
#include <stdint.h>
#include "systimer.h"
/*******************************************************************************
Macros
*******************************************************************************/
#define SDA                                         BIT6
#define SCL                                         BIT7
#define SEL_0                                       P1->SEL0
#define SEL_1                                       P1->SEL1

#define AM2320_I2C_DATA_READ_BYTES                  (7)
#define AM2320_BITBANG_ADDRESS                      (0xB8)
#define AM2320_FUNCTION_CODE                        0x03
#define AM2320_START_ADDRESS                        0x00
#define AM2320_REGISTER_LENGTH                      0x04
/*******************************************************************************
Function Prototype
*******************************************************************************/
void am2320_i2c_init();
void am2320_i2c_write_wakeup();
void am2320_i2c_write_data(uint8_t function_code ,uint8_t start_address ,uint8_t registerlength,uint8_t length_data);
void am2320_i2c_read_data(uint8_t bytes_read);
void am2320_clear_i2c_buffer();
void am2320_temparutre_humidity_measurement();

#endif /* AM2320_I2C_H_ */
