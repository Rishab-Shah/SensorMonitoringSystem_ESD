/*
 * spi.h
 *
 *  Created on: 20-Apr-2021
 *      Author: HP
 */

#ifndef LCD_SPI_H_
#define LCD_SPI_H_

#include "pcd8544.h"
#include "msp.h"
#include <stdint.h>
#include <stdio.h>
#include "systimer.h"

void spi_init();
void spi_config();
void clear_lcd();
void init_lcd();
void write_char_to_lcd(uint8_t ch);
void write_to_lcd(uint8_t dataCommand, uint8_t data);
void set_address(uint8_t xaddress, uint8_t yaddress);
void write_string_to_lcd(const char *string);

#endif /* LCD_SPI_H_ */
