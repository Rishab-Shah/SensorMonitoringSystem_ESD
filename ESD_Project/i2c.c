/*
 * i2c.c
 *
 *  Created on: 11-Apr-2021
 *      Author: HP
 */
#include "i2c.h"


#define I2C_AM2320_ADDRESS          0x5C

#define EUSCI_I2C_I2CSA             EUSCI_B0->I2CSA
#define EUSCI_I2C_BRW               EUSCI_B0->BRW

#define EUSCI_I2C_CTLW0             EUSCI_B0->CTLW0
#define EUSCI_I2C_CTLW0_TR          EUSCI_B_CTLW0_TR
#define EUSCI_I2C_CTLW0_TXSTT       EUSCI_B_CTLW0_TXSTT
#define EUSCI_I2C_CTLW0_TXSTP       EUSCI_B_CTLW0_TXSTP

#define I2C_TXBUF                   EUSCI_B0->TXBUF
#define I2C_RXBUF                   EUSCI_B0->RXBUF

#define EUSCI_I2C_IFG               EUSCI_B0->IFG
#define EUSCI_I2C_IFG_TXIFG0        EUSCI_B_IFG_TXIFG0
#define EUSCI_I2C_IFG_RXIFG0        EUSCI_B_IFG_RXIFG0


uint8_t data_buffer[8];

void delay_usec(int n);

void clear_i2c_buffer()
{
    // Initialize data variable
    uint8_t s = 0;

    for(s = 0; s< 8; s++)
    {
        data_buffer[s] = 0x00;
    }
}

void i2c_init()
{
    clear_i2c_buffer();

    SEL_0 |= SDA | SCL;                // I2C pins

    SEL_1 &= ~SDA;
    SEL_1 &= ~SCL;

    // Configure USCI_B0 for I2C mode
    EUSCI_I2C_CTLW0 |= EUSCI_B_CTLW0_SWRST; // Software reset enabled //EUSCI_B_CTLW0_SWRST

    EUSCI_I2C_CTLW0 = EUSCI_B_CTLW0_SWRST | // Remain eUSCI in reset mode
            EUSCI_B_CTLW0_MODE_3 |          // I2C mode
            EUSCI_B_CTLW0_MST |             // Master mode
            EUSCI_B_CTLW0_SYNC |            // Sync mode
            EUSCI_B_CTLW0_SSEL__SMCLK;      // SMCLK

    EUSCI_I2C_BRW = 70;                     // baudrate = SMCLK / 30 = 100kHz
    EUSCI_I2C_I2CSA = I2C_AM2320_ADDRESS;               // Slave address
    EUSCI_I2C_CTLW0 &= ~EUSCI_B_CTLW0_SWRST;// Release eUSCI from reset

}


void i2c_write_wakeup()
{
    /* Write slave address */
    EUSCI_I2C_I2CSA = I2C_AM2320_ADDRESS;                  // Slave address

    /* Transmit mode */
    EUSCI_I2C_CTLW0 |= EUSCI_I2C_CTLW0_TR;

    /* I2C start condition */
    EUSCI_I2C_CTLW0 |= EUSCI_I2C_CTLW0_TXSTT;

    /* Wait until slave address is sent */
    while(EUSCI_I2C_CTLW0 & EUSCI_I2C_CTLW0_TXSTT);

    while(!(EUSCI_I2C_IFG & EUSCI_I2C_IFG_TXIFG0));

    reset_timer();
    while(delay_msec() < 1);

    /* Transmit stop condition */
    EUSCI_I2C_CTLW0 |= EUSCI_I2C_CTLW0_TXSTP;
    while(EUSCI_I2C_CTLW0 & EUSCI_I2C_CTLW0_TXSTP);
}


void i2c_write_data(uint8_t function_code ,uint8_t start_address ,uint8_t registerlength,uint8_t length_data)
{
    // Initialize data variable
    clear_i2c_buffer();

    data_buffer[0] = function_code;
    data_buffer[1] = start_address;
    data_buffer[2] = registerlength;

    /* Write slave address */
    EUSCI_I2C_I2CSA = I2C_AM2320_ADDRESS;                  // Slave address

    /* Transmit mode */
    EUSCI_I2C_CTLW0 |= EUSCI_I2C_CTLW0_TR;

    /* I2C start condition */
    EUSCI_I2C_CTLW0 |= EUSCI_I2C_CTLW0_TXSTT;

    /* Wait until slave address is sent */
    while(EUSCI_I2C_CTLW0 & EUSCI_B_CTLW0_TXSTT);

    reset_timer();
    while(delay_msec() < 1);

    while(!(EUSCI_I2C_IFG & EUSCI_I2C_IFG_TXIFG0));

    /* write logic for multiple bytes */
    uint8_t i = 0;

    for(i = 0; i<length_data; i++)
    {
        delay_usec(2);
        I2C_TXBUF = data_buffer[i];
        while(!(EUSCI_I2C_IFG & EUSCI_I2C_IFG_TXIFG0));
    }

    /* wait till all the data is transmitted */
    while(!(EUSCI_I2C_IFG & EUSCI_I2C_IFG_TXIFG0));

    /* Transmit stop condition */
    EUSCI_I2C_CTLW0 |= EUSCI_I2C_CTLW0_TXSTP;
    while(EUSCI_I2C_CTLW0 & EUSCI_I2C_CTLW0_TXSTP);
}


void i2c_read_data(uint8_t bytes_read)
{

    uint8_t i = 0;

    // Initialize data variable
    clear_i2c_buffer();

    /* Write slave address */
    EUSCI_I2C_I2CSA = I2C_AM2320_ADDRESS;                  // Slave address

    /* Receiver mode */
    EUSCI_I2C_CTLW0 &= ~EUSCI_I2C_CTLW0_TR;

    /* I2C start condition */
    EUSCI_I2C_CTLW0 |= EUSCI_I2C_CTLW0_TXSTT;

    /* Wait until slave address is sent */
    while(EUSCI_I2C_CTLW0 & EUSCI_I2C_CTLW0_TXSTT);

    delay_usec(3);

    /* read data 8 bytes */
    for(i = 0; i<bytes_read;i++)
    {
       delay_usec(2);
       while(!(EUSCI_I2C_IFG & EUSCI_I2C_IFG_RXIFG0));  /* wait till data is received */
       data_buffer[i] = I2C_RXBUF;
    }

    /* Transmit stop condition */
    EUSCI_I2C_CTLW0 |= EUSCI_I2C_CTLW0_TXSTP;

    while(!(EUSCI_I2C_IFG & EUSCI_I2C_IFG_RXIFG0));  /* wait till data is received */
    data_buffer[i] = I2C_RXBUF;

    while(EUSCI_I2C_CTLW0 & EUSCI_I2C_CTLW0_TXSTP);
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

