/******************************************************************************
* @file: si7021_i2c.h
*
* @brief: This files consists of the function prototypes and macros used in the
* si7021_i2c.c file
* This file Consists of the code for read, write and temperature and
* humidity calculations for the SI7021 sensor
* @author: Rishab Shah
* @date:  23-Apr-2021
* @reference: William Goh - msp432p401x_euscib0_i2c_10
*******************************************************************************/
#ifndef SI7021_I2C_H_
#define SI7021_I2C_H_
/*******************************************************************************
Header files
*******************************************************************************/
#include "msp.h"
#include <stdint.h>
#include "systimer.h"
/*******************************************************************************
Function Prototype
*******************************************************************************/
void si7021_i2c_init();
void si7021_temperature_humidity_measurement();

#endif /* SI7021_I2C_H_ */

/* EOF */
