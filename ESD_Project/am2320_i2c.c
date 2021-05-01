/******************************************************************************
* @file: am2320_i2c.h
*
* @brief: This files consists of the function definitions used in the am2320_i2c.c file
* This file was incorporated to write read,write api's for the am2320 sensor
* @date:  25-Apr-2021
* @reference: William Goh - msp432p401x_euscia0_uart_01
*******************************************************************************/
/*******************************************************************************
Header files
*******************************************************************************/
#include "am2320_i2c.h"
#include "i2c_bitbang.h"
/*******************************************************************************
Macros
*******************************************************************************/
#define AM2320_I2C_ADDRESS          0x5C

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
/*******************************************************************************
Global variables
*******************************************************************************/
extern float am2320_temp_in_degC;
extern float am2320_humidity;
uint8_t am2320_databuffer[8];
/*******************************************************************************
Function prototype
*******************************************************************************/
void delay_usec(int n);
/*******************************************************************************
* @Function: am2320_clear_i2c_buffer
* @Description: clear the i2c bufer for am2320
* @input param : none
* @return: none
*******************************************************************************/
void am2320_clear_i2c_buffer()
{
    /* Initialize data variable */
    uint8_t i = 0;

    for(i = 0; i < 8; i++)
    {
        am2320_databuffer[i] = 0x00;
    }
}

/*******************************************************************************
* @Function: am2320_i2c_init
* @Description: init the am2320 i2c pins
* @input param : none
* @return: none
*******************************************************************************/
void am2320_i2c_init()
{
    /* Initialize data storage variable */
    am2320_clear_i2c_buffer();

    /* I2C pins */
    SEL_0 |= SDA | SCL;

    SEL_1 &= ~SDA;
    SEL_1 &= ~SCL;

    /* Configure USCI_B0 for I2C mode */
    EUSCI_I2C_CTLW0 |= EUSCI_B_CTLW0_SWRST;

    /*
     * Remain eUSCI in reset mode
     * I2C mode
     * Master mode
     * Sync mode
     * SMCLK -> 12 Mhz
     */
    EUSCI_I2C_CTLW0 = EUSCI_B_CTLW0_SWRST |
            EUSCI_B_CTLW0_MODE_3 |
            EUSCI_B_CTLW0_MST |
            EUSCI_B_CTLW0_SYNC |
            EUSCI_B_CTLW0_SSEL__SMCLK;


    /* 300 (40K hz when 12Mhz is freq)
     * 70 when 3Mhz freq
     *  baudrate = SMCLK / 30 = 100kHz */
    EUSCI_I2C_BRW = 70;

    /* Slave address */
    EUSCI_I2C_I2CSA = AM2320_I2C_ADDRESS;

    /* Release eUSCI from reset */
    EUSCI_I2C_CTLW0 &= ~EUSCI_B_CTLW0_SWRST;

}

/*******************************************************************************
* @Function: am2320_i2c_write_wakeup
* @Description: generates the wakeup sequence for the device
* @input param : none
* @return: none
*******************************************************************************/
void am2320_i2c_write_wakeup()
{
    /* Write slave address */
    EUSCI_I2C_I2CSA = AM2320_I2C_ADDRESS;

    /* Transmit mode */
    EUSCI_I2C_CTLW0 |= EUSCI_I2C_CTLW0_TR;

    /* I2C start condition */
    EUSCI_I2C_CTLW0 |= EUSCI_I2C_CTLW0_TXSTT;

    /* Wait until slave address is sent */
    while(EUSCI_I2C_CTLW0 & EUSCI_I2C_CTLW0_TXSTT);

    while(!(EUSCI_I2C_IFG & EUSCI_I2C_IFG_TXIFG0));

    /* Introduce a delay as per the dtasheet to
     * wait for minimum 800 usec */
    reset_timer();
    while(delay_msec() < 1);

    /* Transmit stop condition */
    EUSCI_I2C_CTLW0 |= EUSCI_I2C_CTLW0_TXSTP;
    while(EUSCI_I2C_CTLW0 & EUSCI_I2C_CTLW0_TXSTP);
}

/*******************************************************************************
* @Function: am2320_i2c_write_data
* @Description: Writes data frame to the sensor
* @input param 1: Function code
* @input param 1: Start address in the sensor memory
* @input param 1: length of the register
* @input param 1: No of bytes of data to be sent on the frame
* @return: none
*******************************************************************************/
void am2320_i2c_write_data(uint8_t function_code ,uint8_t start_address ,uint8_t registerlength,uint8_t length_data)
{
    /* Initialize data storage variable */
    am2320_clear_i2c_buffer();

    am2320_databuffer[0] = function_code;
    am2320_databuffer[1] = start_address;
    am2320_databuffer[2] = registerlength;

    /* Write slave address */
    EUSCI_I2C_I2CSA = AM2320_I2C_ADDRESS;                  // Slave address

    /* Transmit mode */
    EUSCI_I2C_CTLW0 |= EUSCI_I2C_CTLW0_TR;

    /* I2C start condition */
    EUSCI_I2C_CTLW0 |= EUSCI_I2C_CTLW0_TXSTT;

    /* Wait until slave address is sent */
    while(EUSCI_I2C_CTLW0 & EUSCI_I2C_CTLW0_TXSTT);

    /* Wait for ~1 msec */
    reset_timer();
    while(delay_msec() < 1);

    while(!(EUSCI_I2C_IFG & EUSCI_I2C_IFG_TXIFG0));

    /* write logic for multiple bytes */
    uint8_t i = 0;

    for(i = 0; i<length_data; i++)
    {
        /* wait as per the arduino timing analysis */
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

/*******************************************************************************
* @Function: am2320_i2c_read_data
* @Description: Read the data from the sensor as per the number of bytes requested
* @input param 1: No of bytes requested
* @return: none
*******************************************************************************/
void am2320_i2c_read_data(uint8_t bytes_read)
{
    uint8_t i = 0;

    /* Initialize data storage variable */
    am2320_clear_i2c_buffer();

    /* Write slave address */
    EUSCI_I2C_I2CSA = AM2320_I2C_ADDRESS;

    /* Receiver mode */
    EUSCI_I2C_CTLW0 &= ~EUSCI_I2C_CTLW0_TR;

    /* I2C start condition */
    EUSCI_I2C_CTLW0 |= EUSCI_I2C_CTLW0_TXSTT;

    /* Wait until slave address is sent */
    while(EUSCI_I2C_CTLW0 & EUSCI_I2C_CTLW0_TXSTT);

    /* wait as per the arduino timing analysis */
    delay_usec(3);

    /* read data 8 bytes */
    for(i = 0; i<bytes_read;i++)
    {
       /* wait as per the arduino timing analysis */
       delay_usec(2);
       while(!(EUSCI_I2C_IFG & EUSCI_I2C_IFG_RXIFG0));  /* wait till data is received */
       am2320_databuffer[i] = I2C_RXBUF;
    }

    /* Transmit stop condition */
    EUSCI_I2C_CTLW0 |= EUSCI_I2C_CTLW0_TXSTP;

    /* wait till data is received */
    while(!(EUSCI_I2C_IFG & EUSCI_I2C_IFG_RXIFG0));
    am2320_databuffer[i] = I2C_RXBUF;

    /* wait for stop bit to be sent */
    while(EUSCI_I2C_CTLW0 & EUSCI_I2C_CTLW0_TXSTP);
}

/*******************************************************************************
* @Function: am2320_temparutre_humidity_measurement
* @Description: Handles the whole 3 step sequence for the AM2320 sensor
* @input param : none
* @return: none
*******************************************************************************/
void am2320_temparutre_humidity_measurement()
{
    /* send the wake up sequence */
    am2320_i2c_write_operation_wakeup(AM2320_BITBANG_ADDRESS);

    /* Delay introduced as per datasheet requirment */
    reset_timer();
    while(delay_msec() < 5);

    /* configure the I2C for AM2320 */
    am2320_i2c_init();

    /* Delay introduced as per datasheet requirment */
    reset_timer();
    while(delay_msec() < 5);

    /* Write command to request for temperature and humidity data  as per datasheet */
    am2320_i2c_write_data(AM2320_FUNCTION_CODE,AM2320_START_ADDRESS,AM2320_REGISTER_LENGTH,3);

    /* Delay introduced as per datasheet requirment */
    reset_timer();
    while(delay_msec() < 2);

    /* configure the I2C for AM2320 */
    am2320_i2c_init();

    /* Read the temperature and humidity data as per datasheet */
    am2320_i2c_read_data(AM2320_I2C_DATA_READ_BYTES);

    /* extract the data updated in the am2320 databuffer */
    am2320_temp_in_degC = (((am2320_databuffer[4] & 0x7F) << 8) + am2320_databuffer[5]) / 10.0;
    am2320_temp_in_degC = (am2320_databuffer[4] & 0x80) ? -am2320_temp_in_degC : am2320_temp_in_degC;
    am2320_humidity = ((am2320_databuffer[2] << 8) + am2320_databuffer[3]) / 10.0;

    /* print the value on console for debugging purpose */
    printf("RH::%f\n",am2320_temp_in_degC);
    printf("T::%f\n",am2320_humidity);

    /* wait for 10 msec before next sequence is started */
    /* Delay introduced as per datasheet requirment */
    reset_timer();
    while(delay_msec() < 10);
}
