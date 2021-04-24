/*
 * systimer.h
 *
 *  Created on: 11-Apr-2021
 *      Author: HP
 */

#ifndef SYSTIMER_H_
#define SYSTIMER_H_

#include "msp.h"
#include <stdint.h>

#define MSEC_1              1
#define MSEC_10             10
#define MSEC_100            100
#define MSEC_1000           1000

#define RESOLUTION          MSEC_1


void TA0_0_IRQHandler(void);
void init_timer();
void reset_timer();
int delay_msec();
void delay_usec(int n);
#endif /* SYSTIMER_H_ */
