/*
 * si7021_i2c.c
 *
 *  Created on: 23-Apr-2021
 *      Author: HP
 */

#include "si7021_i2c.h"

#define I2C_SI7021_ADDRESS                 0x40

#define SI7021_EUSCI_I2C_I2CSA             EUSCI_B1->I2CSA
#define SI7021_EUSCI_I2C_BRW               EUSCI_B1->BRW

#define SI7021_EUSCI_I2C_CTLW0             EUSCI_B1->CTLW0
#define SI7021_EUSCI_I2C_CTLW0_TR          EUSCI_B_CTLW0_TR
#define SI7021_EUSCI_I2C_CTLW0_TXSTT       EUSCI_B_CTLW0_TXSTT
#define SI7021_EUSCI_I2C_CTLW0_TXSTP       EUSCI_B_CTLW0_TXSTP

#define SI7021_I2C_TXBUF                   EUSCI_B1->TXBUF
#define SI7021_I2C_RXBUF                   EUSCI_B1->RXBUF

#define SI7021_EUSCI_I2C_IFG               EUSCI_B1->IFG
#define SI7021_EUSCI_I2C_IFG_TXIFG         EUSCI_B_IFG_TXIFG
#define SI7021_EUSCI_I2C_IFG_RXIFG         EUSCI_B_IFG_RXIFG

uint8_t si7021_databuffer[8];

void read_temperature();
void read_humidity();

extern float si7021_temp_in_degC;
extern float si7021_humidity;

void si7021_i2c_init()
{
    si7021_clear_i2c_buffer();

    // SCL -> P6.5  UCB1SCL
    // SDA -> P6.4  UCB1SDA
    SI7021_SEL_0 |= SI7021_SDA | SI7021_SCL;

    SI7021_SEL_1 &= ~SI7021_SDA;
    SI7021_SEL_1 &= ~SI7021_SCL;
    /* pin selection complete */

    // Configure USCI_B1 for I2C mode
    SI7021_EUSCI_I2C_CTLW0 |= EUSCI_B_CTLW0_SWRST;      // Software reset enabled

    SI7021_EUSCI_I2C_CTLW0 = EUSCI_B_CTLW0_SWRST |      // Remain eUSCI in reset mode
            EUSCI_B_CTLW0_MODE_3 |                      // I2C mode
            EUSCI_B_CTLW0_MST |                         // Master mode
            EUSCI_B_CTLW0_SYNC |                        // Sync mode
            EUSCI_B_CTLW0_SSEL__SMCLK;                  // SMCLK

    SI7021_EUSCI_I2C_BRW = 120;                        // baudrate = 12Mhz/1200 => 10KHz
    //can also be /120 -> 100Khz as max is 400Khz
    SI7021_EUSCI_I2C_I2CSA = I2C_SI7021_ADDRESS;        // Slave address

    SI7021_EUSCI_I2C_CTLW0 &= ~EUSCI_B_CTLW0_SWRST;     // Release eUSCI from reset

}


void si7021_clear_i2c_buffer()
{
    /* Initialize data variable */
    uint8_t s = 0;

    for(s = 0; s < 8; s++)
    {
       si7021_databuffer[s] = 0x00;
    }
}


void si7021_i2c_write_data(uint8_t function_code,uint8_t length_data)
{
    // Initialize data variable
    si7021_clear_i2c_buffer();

    si7021_databuffer[0] = function_code;

    /* Write slave address */
    SI7021_EUSCI_I2C_I2CSA = I2C_SI7021_ADDRESS;   // Slave address

    /* Transmit mode */
    SI7021_EUSCI_I2C_CTLW0 |= SI7021_EUSCI_I2C_CTLW0_TR;

    /* I2C start condition */
    SI7021_EUSCI_I2C_CTLW0 |= SI7021_EUSCI_I2C_CTLW0_TXSTT;

    /* Wait until slave address is sent */
    while(SI7021_EUSCI_I2C_CTLW0 & SI7021_EUSCI_I2C_CTLW0_TXSTT);

    reset_timer();
    while(delay_msec() < 1);

    while(!(SI7021_EUSCI_I2C_IFG & SI7021_EUSCI_I2C_IFG_TXIFG));

    /* write logic for multiple bytes */
    int i = 0;

    for(i = 0; i<length_data; i++)
    {
        //delay_usec(5);
        delay_usec(3);
        SI7021_I2C_TXBUF = si7021_databuffer[i];
        while(!(SI7021_EUSCI_I2C_IFG & SI7021_EUSCI_I2C_IFG_TXIFG));
    }

    /* wait till all the data is transmitted */
    while(!(SI7021_EUSCI_I2C_IFG & SI7021_EUSCI_I2C_IFG_TXIFG));

    /* Transmit stop condition */
    SI7021_EUSCI_I2C_CTLW0 |= SI7021_EUSCI_I2C_CTLW0_TXSTP;
    while(SI7021_EUSCI_I2C_CTLW0 & SI7021_EUSCI_I2C_CTLW0_TXSTP);
}

void si7021_i2c_read_data(uint8_t bytes_read)
{
    uint8_t i = 0;

    /* Initialize data variable */
    si7021_clear_i2c_buffer();

    /* Write slave address */
    SI7021_EUSCI_I2C_I2CSA = I2C_SI7021_ADDRESS;                  // Slave address

    /* Receiver mode */
    SI7021_EUSCI_I2C_CTLW0 &= ~SI7021_EUSCI_I2C_CTLW0_TR;

    /* I2C start condition */
    SI7021_EUSCI_I2C_CTLW0 |= SI7021_EUSCI_I2C_CTLW0_TXSTT;

    /* Wait until slave address is sent */
    while(SI7021_EUSCI_I2C_CTLW0 & SI7021_EUSCI_I2C_CTLW0_TXSTT);

    delay_usec(1);

    /* read data 8 bytes */
    for(i = 0; i < bytes_read;i++)
    {
       //delay_usec(2);
       delay_usec(3);
       while(!(SI7021_EUSCI_I2C_IFG & SI7021_EUSCI_I2C_IFG_RXIFG));  /* wait till data is received */
       si7021_databuffer[i] = SI7021_I2C_RXBUF;
    }

    /* Transmit stop condition */
    SI7021_EUSCI_I2C_CTLW0 |= SI7021_EUSCI_I2C_CTLW0_TXSTP;

    while(!(SI7021_EUSCI_I2C_IFG & SI7021_EUSCI_I2C_IFG_RXIFG));  /* wait till data is received */
    si7021_databuffer[i] = SI7021_I2C_RXBUF;

    while(SI7021_EUSCI_I2C_CTLW0 & SI7021_EUSCI_I2C_CTLW0_TXSTP);
}


void si7021_temperature_humidity_measurement()
{
    /* Write operation */
    si7021_i2c_write_data(SI7021_MEAS_RH_NOHOLD_CMD,1);

    reset_timer();
    while(delay_msec() < 20);

    /* Read operation */
    si7021_i2c_read_data(2);

    read_humidity();

    delay_usec(125);

    /* Write operation */
    si7021_i2c_write_data(SI7021_MEAS_TEMP_NOHOLD_CMD,1);

    reset_timer();
    while(delay_msec() < 20);

    /* Read operation */
    si7021_i2c_read_data(2);

    read_temperature();

}

void read_humidity()
{
    int hum;
    float humidity;

    hum = ((si7021_databuffer[0] << 8) | si7021_databuffer[1]);

    humidity = hum;
    humidity *= 125;
    humidity /= 65536;
    humidity -= 6;
    si7021_humidity = humidity;
}

void read_temperature()
{
    int temp;
    float temperature;

    temp = ((si7021_databuffer[0] << 8) | si7021_databuffer[1]);

    temperature = temp;
    temperature *= 175.72;
    temperature /= 65536;
    temperature -= 46.85;
    si7021_temp_in_degC = temperature;
}
