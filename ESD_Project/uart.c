/******************************************************************************
* @file: uart.c
*
* @brief: This files consists of the init and isr related parameters for uart
* communication.
* This file is required to send data over UART to the PC terminal
* @ Used to implement serial communication over UART
* @author: Rishab Shah
* @date:  20-Apr-2021
* @reference: William Goh - msp432p401x_euscia0_uart_01
*******************************************************************************/
/*******************************************************************************
Header files
*******************************************************************************/
#include "uart.h"
/*******************************************************************************
Global variables
*******************************************************************************/
CB_t cbfifo_tx;
CB_t cbfifo_rx;
/*******************************************************************************
Macros
*******************************************************************************/
#define EUSCI_UART_IFG                                  EUSCI_A0->IFG
#define UART_RXBUF                                      EUSCI_A0->RXBUF
#define UART_TXBUF                                      EUSCI_A0->TXBUF
#define UART_IE                                         EUSCI_A0->IE

#define EUSCI_UART_CTLW0                                EUSCI_A0->CTLW0
#define EUSCI_UART_BRW                                  EUSCI_A0->BRW
#define EUSCI_UART_MCTLW                                EUSCI_A0->MCTLW
/*******************************************************************************
* @Function EUSCIA0_IRQHandler
* @Description: UART interrupt service routine
* @input param : none
* @return: none
*******************************************************************************/
void EUSCIA0_IRQHandler(void)
{
    uint8_t temp_char;

    if (EUSCI_UART_IFG & EUSCI_A_IFG_RXIFG)
    {
        /* Echo the received character back */
        temp_char = UART_RXBUF;

        cbfifo_enqueue(&cbfifo_rx,&temp_char,1);
    }

    if(EUSCI_UART_IFG & EUSCI_A_IFG_TXIFG)
    {
        if(cbfifo_dequeue(&cbfifo_tx,&temp_char,1))
        {
            /* transmit the byte */
            UART_TXBUF = temp_char;
        }
        else
        {
            /* queue is empty so disable transmitter interrupt */
            UART_IE &= ~EUSCI_A_IE_TXIE;
        }

    }
}

/*******************************************************************************
* @Function clock_init
* @Description: Init of clock for uart baud rate setting
* @input param : None
* @return: None
* @reference: TI resource explorer for baudrate settings
*******************************************************************************/
void clock_init()
{
    CS->KEY = CS_KEY_VAL;                   // Unlock CS module for register access

    CS->CTL0 = 0;                           // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_3;           // Set DCO to 12MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELA_2 |             // Select ACLK = REFO
            CS_CTL1_SELS_3 |                // SMCLK = DCO
            CS_CTL1_SELM_3;                 // MCLK = DCO

    CS->KEY = 0;                            // Lock CS module from unintended accesses
}

/*******************************************************************************
* @Function config_uart
* @Description: configure uart for baudrate and interrupt mode along with circular
* buffer
* @input param : None
* @return: None
* @reference: TI resource explorer for baudrate settings
*******************************************************************************/
void config_uart()
{
    /* Configure UART pins
     * set 2-UART pin as secondary function*/
    P1->SEL0 |= BIT2 | BIT3;

    /* Configure UART */
    EUSCI_UART_CTLW0 |= EUSCI_A_CTLW0_SWRST;

    /* Remain eUSCI in reset
     * Configure eUSCI clock source for SMCLK */
    EUSCI_UART_CTLW0 = EUSCI_A_CTLW0_SWRST |
           EUSCI_B_CTLW0_SSEL__SMCLK;

    /* Baud Rate calculation
     12000000/(16*9600) = 78.125
     Fractional portion = 0.125
     User's Guide Table 21-4: UCBRSx = 0x10
     UCBRFx = int ( (78.125-78)*16) = 2
     12000000/16/9600 */
    EUSCI_UART_BRW = 78;
    EUSCI_UART_MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) |
           EUSCI_A_MCTLW_OS16;

    /* Initialize eUSCI */
    EUSCI_UART_CTLW0 &= ~EUSCI_A_CTLW0_SWRST;

    /* Clear eUSCI RX interrupt flag */
    EUSCI_UART_IFG  &= ~EUSCI_A_IFG_RXIFG;

    /* Enable USCI_A0 RX & Tx interrupt */
    UART_IE |= EUSCI_A_IE_RXIE;

    /* Initialize both the separate tx and rx queues */
    cbfifo_init(&cbfifo_tx,CBFIFO_SIZE);
    cbfifo_init(&cbfifo_rx,CBFIFO_SIZE);

    /* Enable eUSCIA0 interrupt in NVIC module */
    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);
}

/* EOF */
