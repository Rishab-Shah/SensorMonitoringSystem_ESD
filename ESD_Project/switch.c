/******************************************************************************
* @file: switch.c
*
* @brief: This files consists of the function headers used in the switch.c file
* @ Used to detect switch as an interrupt
* This file is used to provide control the polling rate at which the data from
* the sensor will be read.
* @author: Rishab Shah
* @date:  13-Apr-2021
* @reference: William Goh - msp432p401x_p1_03
*******************************************************************************/
/*******************************************************************************
Header files
*******************************************************************************/
#include "switch.h"
/*******************************************************************************
Global variables
*******************************************************************************/
extern int32_t T_RH_poll_frequency;
/*******************************************************************************
Macros
*******************************************************************************/
#define POLL_RATE_INC_DEC                                   (1000)
#define POLL_RATE_MAX_VAL                                   (10000)
/*******************************************************************************
Function Prototype
*******************************************************************************/
static void switch_interrupt_init();
/*******************************************************************************
Function definition
*******************************************************************************/
/*******************************************************************************
* @Function switch_init
* @Description: init switch for input pull-up (P1.1 and P1.4)
* @input param : None
* @return: None
*******************************************************************************/
void switch_init()
{
    /* Put switch 1.4 detection in input mode */
    P1->DIR &= ~BIT4;
    P1->OUT |= BIT4;

    /* Put switch 1.1 detection in input mode */
    P1->DIR &= ~BIT1;
    P1->OUT |= BIT1;

    /* Enable pull-up resistor (P1.4 output high) */
    P1->REN |= BIT4;
    /* Enable pull-up resistor (P1.1 output high) */
    P1->REN |= BIT1;

    /* Port multiplexing - select switch mode */
    P2->SEL0 &= ~(BIT4);
    P2->SEL1 &= ~(BIT4);

    /* Port multiplexing - select switch mode */
    P2->SEL0 &= ~(BIT1);
    P2->SEL1 &= ~(BIT1);

    switch_interrupt_init();
}

/*******************************************************************************
* @Function switch_interrupt_init
* @Description: init led for switch detection - interrupt part
* @input param : None
* @return: None
*******************************************************************************/
static void switch_interrupt_init()
{
    /* Interrupt on high-to-low transition */
    P1->IES |= BIT4;
    P1->IES |= BIT1;

    /* Clear all P1 interrupt flags */
    P1->IFG = 0;

    /* Enable interrupt */
    P1->IE |= BIT4;
    P1->IE |= BIT1;

    /* Enable Port 1 interrupt on the NVIC */
    NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);
}

/*******************************************************************************
* @Function PORT1_IRQHandler
* @Description: IRQ handler for Port 1. Used to detect switch press P1.1 and P1.4
* @input param : None
* @return: None
*******************************************************************************/
void PORT1_IRQHandler(void)
{
    volatile uint32_t i;
    int32_t temp_value = 0;

    /* Delay for switch debounce (S/W) */
    for(i = 0; i < 10000; i++)
        ;

    /* Check for interrupt on Pin 4 */
    if(P1->IFG & BIT4)
    {
        /* Increase by 1000 */
        temp_value = T_RH_poll_frequency + POLL_RATE_INC_DEC;

        if(temp_value > POLL_RATE_MAX_VAL)
        {
            temp_value = POLL_RATE_MAX_VAL;
        }
    }

    if(P1->IFG & BIT1)
    {
        /* decrease by 1000 */
        temp_value = T_RH_poll_frequency - POLL_RATE_INC_DEC;

        if(temp_value < T_RH_BASE_POLL_FREQ)
        {
            temp_value = T_RH_BASE_POLL_FREQ;
        }
    }

    /* clear P1.4 switch interrupt */
    P1->IFG &= ~BIT4;

    /* clear P1.1 switch interrupt */
    P1->IFG &= ~BIT1;

    /* Update the polling frequency */
    T_RH_poll_frequency = temp_value;
}

/* EOF */
