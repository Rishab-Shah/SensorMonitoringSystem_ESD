/*
 * si7021.h
 *
 *  Created on: 23-Apr-2021
 *      Author: HP
 */

#ifndef SI7021_I2C_H_
#define SI7021_I2C_H_

#include "msp.h"
#include <stdint.h>
#include "systimer.h"


// SCL -> P6.5  UCB1SCL
// SDA -> P6.4  UCB1SDA

#define SI7021_SDA                          BIT4
#define SI7021_SCL                          BIT5
#define SI7021_SEL_0                        P6->SEL0
#define SI7021_SEL_1                        P6->SEL1


#define SI7021_MEAS_RH_NOHOLD_CMD           0xF5                 /**< Measure Relative Humidity, No Hold Master Mode */
#define SI7021_MEAS_TEMP_NOHOLD_CMD         0xF3                 /**< Measure Temperature, No Hold Master Mode */

void si7021_i2c_init();
void si7021_i2c_write_data(uint8_t function_code,uint8_t length_data);
void si7021_i2c_read_data(uint8_t bytes_read);
void si7021_clear_i2c_buffer();
void si7021_temperature_humidity_measurement();

#endif /* SI7021_I2C_H_ */
