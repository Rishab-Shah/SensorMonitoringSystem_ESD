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
#include "switch.h"
/*******************************************************************************
Global variables
*******************************************************************************/
extern int32_t am2320_poll_frequency;
/*******************************************************************************
Function definition
*******************************************************************************/
/*******************************************************************************
* @Function switch_init
* @Description: init switch for input pullup (P1.1 and P1.4)
* @input param : None
* @return: None
*******************************************************************************/
void switch_init()
{
    P1->DIR &= ~BIT4;
    P1->OUT |= BIT4;

    P1->DIR &= ~BIT1;
    P1->OUT |= BIT1;

    P1->REN |= BIT4;                         // Enable pull-up resistor (P1.1 output high)
    P1->REN |= BIT1;

    /* Port multiplexing - select switch mode */
    P2->SEL0 &= ~(BIT4);
    P2->SEL1 &= ~(BIT4);

    P2->SEL0 &= ~(BIT1);
    P2->SEL1 &= ~(BIT1);

}

/*******************************************************************************
* @Function led_port_init
* @Description: init led for switch detection indication
* @input param : None
* @return: None
*******************************************************************************/
void led_port_init()
{
    // Configure GPIO - Red
    P2->DIR |= BIT0;

    // Configure GPIO - Green
    P2->DIR |= BIT1;

    // Configure GPIO - Blue
    P2->DIR |= BIT2;

    // Selection option
    P2->SEL0 |= (BIT0);
    P2->SEL1 &= ~(BIT0);

    P2->SEL0 |= (BIT1);
    P2->SEL1 &= ~(BIT1);

    P2->SEL0 |= (BIT2);
    P2->SEL1 &= ~(BIT2);

    P2->OUT &= ~BIT0;
    P2->OUT &= ~BIT1;
    P2->OUT &= ~BIT2;
}

/*******************************************************************************
* @Function switch_interrupt_init
* @Description: init led for switch detection - interrupt part
* @input param : None
* @return: None
*******************************************************************************/
void switch_interrupt_init()
{
    P1->IES |= BIT4;                        // Interrupt on high-to-low transition
    P1->IES |= BIT1;                        // Interrupt on high-to-low transition

    P1->IFG = 0;                            // Clear all P1 interrupt flags

    P1->IE |= BIT4;                         // Enable interrupt for P1.1
    P1->IE |= BIT1;

    // Enable Port 1 interrupt on the NVIC
    NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);
}

/*******************************************************************************
* @Function PORT1_IRQHandler
* @Description: IRQ handler for Port 1
* @input param : None
* @return: None
*******************************************************************************/
void PORT1_IRQHandler(void)
{
    volatile uint32_t i;
    int32_t temp_value = 0;

    /* Delay for switch debounce */
    for(i = 0; i < 10000; i++)
        ;

    /* Check for interrupt on Pin 4 */
    if(P1->IFG & BIT4)
    {
        //P2->OUT ^= BIT2; //toggle blue
        /* Increase by 1000 */
        temp_value = am2320_poll_frequency + INC_PERCENT;

        if(temp_value > 10000)
        {
            temp_value = 10000;
        }
        else
        {
            /* Do nothing */
        }

#if 0
        /* Stop timer */
        TIMER_A0->CTL |= TIMER_A_CTL_MC__STOP;

        /* Set new pwm duty cycle value */
        TIMER_A0->CCR[2] = temp_value;              // Duty cycle (TA0.2)

        /* Start timer and in upcounting mode */
        TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK |             // SMCLK
                TIMER_A_CTL_MC__UP |                          // Up mode
                TIMER_A_CTL_CLR;
#endif
    }
    else
    {
        /* Do Nothing */
    }

    if(P1->IFG & BIT1)
    {
        //P2->OUT ^= BIT1; //toggle green
        /* decrease by 1000 */

        temp_value = am2320_poll_frequency - DEC_PERCENT;

        if(temp_value < 2000)
        {
            temp_value = 2000;
        }
        else
        {
            /* Do nothing */
        }

#if 0
        /* Stop timer */
        TIMER_A0->CTL |= TIMER_A_CTL_MC__STOP;

        /* Set new pwm duty cycle value */
        TIMER_A0->CCR[2] = temp_value;              // Duty cycle (TA0.2)

        /* Start timer and in upcounting mode */
        TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK |             // SMCLK
                TIMER_A_CTL_MC__UP |                          // Up mode
                TIMER_A_CTL_CLR;
#endif
    }
    else
    {
        /* Do Nothing */
    }

    P1->IFG &= ~BIT4;

    P1->IFG &= ~BIT1;

    /* update the current ref value for pwm */
    am2320_poll_frequency = temp_value;
}

/* EOF */
