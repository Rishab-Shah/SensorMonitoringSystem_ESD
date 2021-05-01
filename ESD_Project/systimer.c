/******************************************************************************
* @file: systimer.c
*
* @brief: This files consists of the init ,isr, count and delay related functions
* for timer 0
* This file is used to provide the systick timer feature to carry out time based
* activities
* @author: Rishab Shah
* @date:  12-Apr-2021
*******************************************************************************/
/*******************************************************************************
Header files
*******************************************************************************/
#include "systimer.h"
/*******************************************************************************
 Macros
*******************************************************************************/
#define TIMER_SELECTED                                   (0)
#define TIMER_CTL                                        TIMER_A0->CTL
#define TIMER_CCTL                                       TIMER_A0->CCTL[TIMER_SELECTED]
#define TIMER_CCR                                        TIMER_A0->CCR[TIMER_SELECTED]
/*******************************************************************************
 Global variables
*******************************************************************************/
volatile uint16_t g_count = 0;
volatile uint16_t g_counter2 = 0;
/*******************************************************************************
* @Function init_timer
* @Description: Initializes the timer to run at 1msec interrupt interval
* @input param : none
* @return: none
* @reference: TI resource explorer. Modified the timing count values as per
* the need. Only base frame of configuration was used to save development time.
*******************************************************************************/
void init_timer()
{
    /* Timer configuration */
    TIMER_CCTL &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_CCTL = TIMER_A_CCTLN_CCIE;            // TACCR0 interrupt enabled
    TIMER_CCR = 12000;                          /* 12,000 value provides a resolution of 1 msec */
    TIMER_CTL = TIMER_A_CTL_SSEL__SMCLK |       // SMCLK, UP mode
            TIMER_A_CTL_MC__UP;

    /* Variable to keep track of systicks */
    g_count = 0 ;
    g_counter2 = 0;

    /* Set timer 0 interrupt corresponding number */
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);

    /* Enable global interrupt */
    __enable_irq();
}

/*******************************************************************************
* @Function : TA0_0_IRQHandler
* @Description: Initializes the timer to run at 1msec interrupt interval
* @input param : none
* @return: none
*******************************************************************************/
void TA0_0_IRQHandler(void)
{
    /* Kept two variables to scale teh system in future if
     *  the minimum tick frequency needs to be changed */
    /* Increment at every 1 msec tick */
    g_count++;

    if(g_count > RESOLUTION )
    {
        /* Used for debugging if the systick is runnung */
        //P1->OUT ^= BIT0;// Toggle P1.0 LED
        g_count = 0;
        g_counter2++;
    }
    else
    {
        /* Do nothing */
    }

    /* Clear the compare interrupt flag */
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}

/*******************************************************************************
* @Function : reset_timer
* @Description: Resets the value to start a new read. Acts as a new reference to
* implement any of the required delay
* @input param : none
* @return: none
*******************************************************************************/
void reset_timer()
{
    g_counter2 = 0;
}

/*******************************************************************************
* @Function : delay_msec
* @Description: returns the value continuously till the delay is achieved
* @input param : none
* @return: the tick value
*******************************************************************************/
int delay_msec()
{
    return g_counter2;
}

/*******************************************************************************
* @Function : delay_usec
* @Description: Used to provide delays which are less than 1 msec.
* 1 count = 10usec approx
* @input param : none
* @return: none
*******************************************************************************/
void delay_usec(int n)
{
    int i, j;

    for (j = 0; j < n; j++)
        for (i = 1; i > 0; i--);      /* delay 10usec */
}

/* EOF */
