/******************************************************************************
* @file: gpio.c
*
* @brief: This files consists of the function definitions used in the gpio.c file
* This contains the logic to initialise the heart beat led and fan configuration through PWM led
* @author: Rishab Shah
* @date:  22-Apr-2021
* @reference: William Goh - msp432p401x_euscia0_uart_01
*******************************************************************************/
/*******************************************************************************
Header files
*******************************************************************************/
#include "gpio.h"
/*******************************************************************************
Function definition
*******************************************************************************/
static void heart_beat();
static void fan_config();
/*******************************************************************************
* @Function gpio_init
* @Description: init all the gpio related modules
* @input param : None
* @return: None
*******************************************************************************/
void gpio_init()
{
    heart_beat();

    fan_config();
}

/*******************************************************************************
* @Function heart_beat
* @Description: init  the heart beat led P1.0
* @input param : None
* @return: None
*******************************************************************************/
static void heart_beat()
{
    //Configure GPIO
    P1->DIR |= BIT0;
    P1->OUT &= ~BIT0;

}

/*******************************************************************************
* @Function fan_config
* @Description: init  the fan (BLUE LED) P2.2
* @input param : None
* @return: None
*******************************************************************************/
static void fan_config()
{
    //Configure GPIO
    P2->DIR |= BIT2;

    TURN_FAN_OFF;
}

/* EOF */
