/*
 * i2c.c
 *
 *  Created on: 11-Apr-2021
 *      Author: HP
 */
#include <am2320_i2c.h>


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


uint8_t am2320_databuffer[8];

void delay_usec(int n);

void am2320_clear_i2c_buffer()
{
    // Initialize data variable
    uint8_t s = 0;

    for(s = 0; s< 8; s++)
    {
        am2320_databuffer[s] = 0x00;
    }
}

void am2320_i2c_init()
{
    am2320_clear_i2c_buffer();

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


void am2320_i2c_write_wakeup()
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


void am2320_i2c_write_data(uint8_t function_code ,uint8_t start_address ,uint8_t registerlength,uint8_t length_data)
{
    // Initialize data variable
    am2320_clear_i2c_buffer();

    am2320_databuffer[0] = function_code;
    am2320_databuffer[1] = start_address;
    am2320_databuffer[2] = registerlength;

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
        I2C_TXBUF = am2320_databuffer[i];
        while(!(EUSCI_I2C_IFG & EUSCI_I2C_IFG_TXIFG0));
    }

    /* wait till all the data is transmitted */
    while(!(EUSCI_I2C_IFG & EUSCI_I2C_IFG_TXIFG0));

    /* Transmit stop condition */
    EUSCI_I2C_CTLW0 |= EUSCI_I2C_CTLW0_TXSTP;
    while(EUSCI_I2C_CTLW0 & EUSCI_I2C_CTLW0_TXSTP);
}


void am2320_i2c_read_data(uint8_t bytes_read)
{

    uint8_t i = 0;

    // Initialize data variable
    am2320_clear_i2c_buffer();

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
       am2320_databuffer[i] = I2C_RXBUF;
    }

    /* Transmit stop condition */
    EUSCI_I2C_CTLW0 |= EUSCI_I2C_CTLW0_TXSTP;

    while(!(EUSCI_I2C_IFG & EUSCI_I2C_IFG_RXIFG0));  /* wait till data is received */
    am2320_databuffer[i] = I2C_RXBUF;

    while(EUSCI_I2C_CTLW0 & EUSCI_I2C_CTLW0_TXSTP);
}


void am2320_get_RH_and_temperature(unsigned int *data1, signed int *data2)
{
     *data1 = ((unsigned int)((am2320_databuffer[2] << 8) | am2320_databuffer[3]));
     *data2 = ((am2320_databuffer[4] << 8) | am2320_databuffer[5]);
}


void am2320_get_CRC(unsigned int *CRC_data)
{
     *CRC_data = ((unsigned int)((am2320_databuffer[7] << 8) | am2320_databuffer[6]));
}


unsigned int am2320_CRC16(unsigned char *ptr, uint8_t length)
{
    unsigned int crc = 0xFFFF;
    uint8_t s = 0x00;

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



