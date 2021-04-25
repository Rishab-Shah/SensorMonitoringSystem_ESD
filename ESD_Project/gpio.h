/*
 * gpio.h
 *
 *  Created on: 20-Apr-2021
 *      Author: HP
 */

#ifndef GPIO_H_
#define GPIO_H_

#include "msp.h"
#include <stdint.h>
#include <stdio.h>


void gpio_init();


#define TURN_FAN_ON                              P2->OUT |= BIT2
#define TURN_FAN_OFF                             P2->OUT &= ~BIT2
#endif /* GPIO_H_ */
