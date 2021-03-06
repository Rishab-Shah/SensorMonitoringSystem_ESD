/******************************************************************************
* @file: lcd_spi.h
*
* @brief: This files consists of the function headers used in the lcd_spi.h file
* This file is is used for communication with the LCD module. The reference code
* was for MSP430. The code was ported for the MSP432P401R
* @date:  20-Apr-2021
* @reference: William Goh - msp432p401x_euscib0_spi_09
*  http://rohitg.in/2014/11/09/Nokia5510-lcd-with-MSP430/
*******************************************************************************/
#ifndef LCD_SPI_H_
#define LCD_SPI_H_
/*******************************************************************************
Header files
*******************************************************************************/
#include "pcd8544.h"
#include "msp.h"
#include <stdint.h>
#include <stdio.h>
#include "systimer.h"
/*******************************************************************************
Function prototype
*******************************************************************************/
void spi_init();
void spi_config();
void clear_lcd();
void init_lcd();
void write_char_to_lcd(uint8_t ch);
void write_to_lcd(uint8_t dataCommand, uint8_t data);
void set_address(uint8_t xaddress, uint8_t yaddress);
void write_string_to_lcd(const char *string);

#endif /* LCD_SPI_H_ */

/* EOF */
