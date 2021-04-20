/*
 * i2c.c
 *
 *  Created on: 11-Apr-2021
 *      Author: HP
 */
#include "i2c.h"

#define FUNCTION_CODE       0x03
#define START_ADDRESS       0x00
#define REGISTER_LENGTH     0x04
#define I2C_AM2320_ADDRESS  0x5C

uint8_t data_buffer[8];

void delay_usec(int n);

void clear_i2c_buffer()
{
    // Initialize data variable
    unsigned char s = 0;

    for(s = 0; s< 8; s++)
    {
        data_buffer[s] = 0x00;
    }
}

void i2c_init()
{
    clear_i2c_buffer();

    P1->SEL0 |= BIT6 | BIT7;                // I2C pins

    P1->SEL1 &= ~BIT6;
    P1->SEL1 &= ~BIT7;

    // Configure USCI_B0 for I2C mode
    EUSCI_B0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Software reset enabled

    EUSCI_B0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset mode
            EUSCI_B_CTLW0_MODE_3 |          // I2C mode
            EUSCI_B_CTLW0_MST |             // Master mode
            EUSCI_B_CTLW0_SYNC |            // Sync mode
            EUSCI_B_CTLW0_SSEL__SMCLK;      // SMCLK

    EUSCI_B0->BRW = 30;                     // baudrate = SMCLK / 30 = 100kHz
    EUSCI_B0->I2CSA = I2C_AM2320_ADDRESS;               // Slave address
    EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;// Release eUSCI from reset

}


void i2c_write_wakeup()
{
    /* Write slave address */
    EUSCI_B0->I2CSA = I2C_AM2320_ADDRESS;                  // Slave address

    /* Transmit mode */
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;

    /* I2C start condition */
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;

    /* Wait until slave address is sent */
    while(EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT);

    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));

    //for(i = 300; i>0 ; i--);
    reset_timer();
    while(delay_msec() < 1);


    /* Transmit stop condition */
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
    while(EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTP);
}


void i2c_write_data()
{
    // Initialize data variable
    clear_i2c_buffer();

    data_buffer[0] = FUNCTION_CODE;
    data_buffer[1] = START_ADDRESS;
    data_buffer[2] = REGISTER_LENGTH;

    /* Write slave address */
    EUSCI_B0->I2CSA = I2C_AM2320_ADDRESS;                  // Slave address

    /* Transmit mode */
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;

    /* I2C start condition */
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;

    /* Wait until slave address is sent */
    while(EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT);

    reset_timer();
    while(delay_msec() < 1);

    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));

    /* write logic for multiple bytes */
    uint8_t i = 0;

    for(i = 0; i<3; i++)
    {
        delay_usec(2);
        EUSCI_B0->TXBUF = data_buffer[i];
        while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));
    }

    /* wait till all the data is transmitted */
    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0));

    /* Transmit stop condition */
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
    while(EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTP);
}


void i2c_read_data()
{
    // Initialize data variable
    clear_i2c_buffer();

    /* Write slave address */
    EUSCI_B0->I2CSA = I2C_AM2320_ADDRESS;                  // Slave address

    /* Receiver mode */
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR;

    /* I2C start condition */
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;

    /* Wait until slave address is sent */
    while(EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT);

    /* trying to create a delay of 30usec */
    P1->OUT |= BIT0;

    uint8_t i = 0;
    for(i = 100; i>0;i--);

    P1->OUT &= ~BIT0;

    /* read data 8 bytes */
    for(i = 0; i<7;i++)
    {
       delay_usec(2);
       while(!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0));  /* wait till data is received */
       data_buffer[i] = EUSCI_B0->RXBUF;
    }

    /*Stop part */
    //while(!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0));
    /* Transmit stop condition */
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

    while(!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0));  /* wait till data is received */
    data_buffer[i] = EUSCI_B0->RXBUF;

    /* Transmit mode */
    // EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;

    while(EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTP);
}


void get_RH_and_temperature(unsigned int *data1, signed int *data2)
{
     *data1 = ((unsigned int)((data_buffer[2] << 8) | data_buffer[3]));
     *data2 = ((data_buffer[4] << 8) | data_buffer[5]);
}


void get_CRC(unsigned int *CRC_data)
{
     *CRC_data = ((unsigned int)((data_buffer[7] << 8) | data_buffer[6]));
}


unsigned int CRC16(unsigned char *ptr, unsigned char length)
{
      unsigned int crc = 0xFFFF;
      unsigned char s = 0x00;

      while(length--)
      {
        crc ^= *ptr++;
        for(s = 0; s < 8; s++)
        {
          if((crc & 0x01) != 0)
          {
            crc >>= 1;
            crc ^= 0xA001;
          }
          else
          {
            crc >>= 1;
          }
        }
      }

      return crc;
}


/* 1 count = 10usec approx */
void delay_usec(int n)
{
    int i, j;

    for (j = 0; j < n; j++)
        for (i = 1; i > 0; i--);      /* delay 10usec */
}

