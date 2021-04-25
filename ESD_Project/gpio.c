/******************************************************************************
* @file:switch.c
*
* @brief: This files consists of the function headers used in the switch.c file
*
* @author: Rishab Shah
* @date:  12-Mar-2021
*******************************************************************************/
/*******************************************************************************
Header files
*******************************************************************************/
#include "gpio.h"
/*******************************************************************************
Global variables
*******************************************************************************/

/*******************************************************************************
Function definition
*******************************************************************************/
static void heart_beat();
static void fan_config();
/*******************************************************************************
* @Function switch_init
* @Description: init switch for input pullup (P1.1 and P1.4)
* @input param : None
* @return: None
*******************************************************************************/
void gpio_init()
{
    heart_beat();

    fan_config();
}

static void heart_beat()
{
    //Configure GPIO
    P1->DIR |= BIT0;
    P1->OUT &= ~BIT0;

    P4->DIR |= BIT0;
    P4->OUT &= ~BIT0;

}

static void fan_config()
{
    //Configure GPIO
    P2->DIR |= BIT2;

    TURN_FAN_OFF;
}

/* EOF */
