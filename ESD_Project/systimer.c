/*
 * systimer.c
 *
 *  Created on: 11-Apr-2021
 *      Author: HP
 */

#include "systimer.h"

volatile uint16_t g_count = 0;
volatile uint16_t g_counter2 = 0;


void init_timer()
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    TIMER_A0->CCR[0] = 12000;
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | // SMCLK, UP mode
            TIMER_A_CTL_MC__UP;

    //SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;    // Enable sleep on exit from ISR

    g_count = 0 ;
    g_counter2 = 0;

    // Ensures SLEEPONEXIT takes effect immediately
    //__DSB();

    // Enable global interrupt
    __enable_irq();

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);

}


// Timer A0_0 interrupt service routine
void TA0_0_IRQHandler(void)
{
    g_count++;

    if(g_count > RESOLUTION )
    {
        //P1->OUT ^= BIT0;// Toggle P1.0 LED
        g_count = 0;
        g_counter2++;
    }
    else
    {
        /* Do nothing */
    }

    // Clear the compare interrupt flag
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}

void reset_timer()
{
    g_counter2 = 0;
}

int delay_msec()
{
    return g_counter2;
}

/* 1 count = 10usec approx */
void delay_usec(int n)
{
    int i, j;

    for (j = 0; j < n; j++)
        for (i = 1; i > 0; i--);      /* delay 10usec */
}

