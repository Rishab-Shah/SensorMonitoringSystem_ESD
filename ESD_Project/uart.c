/******************************************************************************
* @file:uart.h
*
* @brief: This files consists of the init and isr related parameters for uart
* commmunication
*
* @author: Rishab Shah
* @date:  12-Mar-2021
*******************************************************************************/
/*******************************************************************************
Header files
*******************************************************************************/
#include "uart.h"
/*******************************************************************************
Global variables
*******************************************************************************/
uint8_t in_storage;
CB_t cbfifo_tx;
CB_t cbfifo_rx;
/*******************************************************************************
Function prototype
*******************************************************************************/
/*******************************************************************************
* @Function EUSCIA0_IRQHandler
* @Description: UART interrupt service routine
* @input param : None
* @return: None
*******************************************************************************/
void EUSCIA0_IRQHandler(void)
{
    //int temp = 0;
    uint8_t out_storage;

    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG)
    {
        /* Echo the received character back */
        in_storage = EUSCI_A0->RXBUF;

        cbfifo_enqueue(&cbfifo_rx,&in_storage,1);
    }

    if(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG)
    {
        if(cbfifo_dequeue(&cbfifo_tx,&out_storage,1))
        {
            /* transmit the byte */
            EUSCI_A0->TXBUF = out_storage;
        }
        else
        {
            /* queue is empty so disable transmitter interrupt */
            EUSCI_A0->IE &= ~EUSCI_A_IE_TXIE;
        }

    }
}

/*******************************************************************************
* @Function clock_init
* @Description: Init of clock for uart baud rate setting
* @input param : None
* @return: None
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
* @Description: configure uart fro baudrate and interrupt
* @input param : None
* @return: None
*******************************************************************************/
void config_uart()
{
    // Configure UART pins
    P1->SEL0 |= BIT2 | BIT3;                            // set 2-UART pin as secondary function

    // Configure UART
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST;             // Put eUSCI in reset

    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST |             // Remain eUSCI in reset
           EUSCI_B_CTLW0_SSEL__SMCLK;                   // Configure eUSCI clock source for SMCLK

    // Baud Rate calculation
    // 12000000/(16*9600) = 78.125
    // Fractional portion = 0.125
    // User's Guide Table 21-4: UCBRSx = 0x10
    // UCBRFx = int ( (78.125-78)*16) = 2
    EUSCI_A0->BRW = 78;                                 // 12000000/16/9600
    EUSCI_A0->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) |
           EUSCI_A_MCTLW_OS16;



    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;            // Initialize eUSCI

    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;                // Clear eUSCI RX interrupt flag

    EUSCI_A0->IE |= EUSCI_A_IE_RXIE /*|EUSCI_A_IFG_TXIFG*/;    // Enable USCI_A0 RX & Tx interrupt

    cbfifo_init(&cbfifo_tx,CBFIFO_SIZE);
    cbfifo_init(&cbfifo_rx,CBFIFO_SIZE);

    // Enable eUSCIA0 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);
}

#if 0
/*******************************************************************************
* @Function Name: __sys_write
* @Description: enqueues the tx buffer to display data on the terminal
* @onput param: None
* @return: None
*******************************************************************************/
int __sys_write(int handle, char *buf, int size)
{
    if(buf == NULL)
    {
        return -1;
    }

    /* Check for the queue to be empty */
    while(cbfifo_full(&cbfifo_tx))
    {
        ;
    }

    cbfifo_enqueue(&cbfifo_tx,buf,size);

    /* enable the transmit interrupt */
    EUSCI_A0->IE |=  EUSCI_A_IFG_TXIFG;
    /*if(!(UART0->C2 & UART0_C2_TIE_MASK))
    {
        UART0->C2 |= UART0_C2_TIE(1);
    }*/

    return 0;
}

/*******************************************************************************
* @Function Name: __sys_readc
* @Description: dequeues the rx buffer to fetch data from the terminal
* @onput param: None
* @return: None
*******************************************************************************/
int __sys_readc(void)
{
    char data_byte;

    /* dequeue one byte at a time */
    if(cbfifo_dequeue(&cbfifo_rx,&data_byte,1) != 1)
    {
        return -1;
    }
    else
    {
        return data_byte;
    }
}
#endif
/* EOF */
