/******************************************************************************
* @file: si7021_i2c.c
*
* @brief: This files consists of the function definitions for the SI7021
* temperature and Humidity sensor
* This file Consists of the code for read, write and temperature and
* humidity calculations for the SI7021 sensor
* @author: Rishab Shah
* @date:  23-Apr-2021
* @reference: William Goh - msp432p401x_euscib0_i2c_10
*******************************************************************************/
#include "si7021_i2c.h"
/*******************************************************************************
Macros
*******************************************************************************/
// SCL -> P6.5  UCB1SCL
// SDA -> P6.4  UCB1SDA
#define SI7021_SDA                         BIT4
#define SI7021_SCL                         BIT5
#define SI7021_SEL_0                       P6->SEL0
#define SI7021_SEL_1                       P6->SEL1

#define SI7021_MEAS_RH_NOHOLD_CMD          0xF5 /* Measure RH. No Hold master mode */
#define SI7021_MEAS_TEMP_NOHOLD_CMD        0xF3 /* Measure Temperature. No Hold master mode */
#define I2C_SI7021_ADDRESS                 0x40 /* Address of the SI7021 */

/* Various macros used to provide flexibility in development */
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

#define SI7021_BUFF_SIZE                   (8)
/*******************************************************************************
Global Variables
*******************************************************************************/
uint8_t si7021_databuffer[SI7021_BUFF_SIZE];
extern float si7021_temp_in_degC;
extern float si7021_humidity;
/*******************************************************************************
Function Prototype
*******************************************************************************/
static void si7021_read_temperature();
static void si7021_read_humidity();
static void si7021_i2c_write_data(uint8_t function_code,uint8_t length_data);
static void si7021_i2c_read_data(uint8_t bytes_read);
static void si7021_clear_i2c_buffer();
/*******************************************************************************
* @Function si7021_i2c_init
* @Description: Initialise the I2c bus for SI7021 TRH sensor
* @input param : None
* @return: None
*******************************************************************************/
void si7021_i2c_init()
{
    si7021_clear_i2c_buffer();

    // SCL -> P6.5  UCB1SCL
    // SDA -> P6.4  UCB1SDA
    SI7021_SEL_0 |= SI7021_SDA | SI7021_SCL;

    SI7021_SEL_1 &= ~SI7021_SDA;
    SI7021_SEL_1 &= ~SI7021_SCL;
    /* pin selection complete */

    /* Configure USCI_B1 for I2C mode */
    SI7021_EUSCI_I2C_CTLW0 |= EUSCI_B_CTLW0_SWRST;      // Software reset enabled

    SI7021_EUSCI_I2C_CTLW0 = EUSCI_B_CTLW0_SWRST |      // Remain eUSCI in reset mode
            EUSCI_B_CTLW0_MODE_3 |                      // I2C mode
            EUSCI_B_CTLW0_MST |                         // Master mode
            EUSCI_B_CTLW0_SYNC |                        // Sync mode
            EUSCI_B_CTLW0_SSEL__SMCLK;                  // SMCLK

    SI7021_EUSCI_I2C_BRW = 120;                        // baudrate /120 -> 100Khz and max is 400Khz

    SI7021_EUSCI_I2C_I2CSA = I2C_SI7021_ADDRESS;       // Slave address

    SI7021_EUSCI_I2C_CTLW0 &= ~EUSCI_B_CTLW0_SWRST;    // Release eUSCI from reset

}

/*******************************************************************************
* @Function si7021_clear_i2c_buffer
* @Description:Clear the data buffer before and every read or write operation
* to ensure there is no error data read
* @input param : None
* @return: None
*******************************************************************************/
static void si7021_clear_i2c_buffer()
{
    /* Initialize data variable */
    uint8_t i = 0;

    for(i = 0; i < 8; i++)
    {
       si7021_databuffer[i] = 0x00;
    }
}

/*******************************************************************************
* @Function si7021_i2c_write_data
* @Description: Performs a write operation to Si7021 device
* @input param 1 : The function code for which the device is to capture data
* - temperature or humidity
* @input param 2 : The amount of data bytes that are to be sent on the I2C frame
* before the stop bit is sent
* @return: none
*******************************************************************************/
static void si7021_i2c_write_data(uint8_t function_code,uint8_t length_data)
{
    /* Initialize data variable */
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

/*******************************************************************************
* @Function si7021_i2c_read_data
* @Description: Performs the read operation for the number of bytes requested
* by the user derived from datasheet
* @input param 1 : no of bytes requested by the user
* @return: none
*******************************************************************************/
static void si7021_i2c_read_data(uint8_t bytes_read)
{
    uint8_t i = 0;

    /* Initialize data variable */
    si7021_clear_i2c_buffer();

    /* Write slave address */
    SI7021_EUSCI_I2C_I2CSA = I2C_SI7021_ADDRESS;

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


/*******************************************************************************
* @Function si7021_temperature_humidity_measurement
* @Description: Performs the whole write and read transaction s for the humidity
* followed by temperature as per the polling rate which is set by the user
* with 1 sec being minimum
* @input param : none
* @return: none
*******************************************************************************/
void si7021_temperature_humidity_measurement()
{
    /* Humidity related operations */
    /* Write operation  with one byte request */
    si7021_i2c_write_data(SI7021_MEAS_RH_NOHOLD_CMD,1);

    /* Wait for 20 msec for the conversion to take place */
    reset_timer();
    while(delay_msec() < 20);

    /* Read operation with 2 byte request*/
    si7021_i2c_read_data(2);

    /* read humidity value */
    si7021_read_humidity();

    delay_usec(125);

    /**********************************************************/

    /* Temperature related operations */
    /* Write operation  with one byte request */
    si7021_i2c_write_data(SI7021_MEAS_TEMP_NOHOLD_CMD,1);

    /* Wait for 20 msec for the conversion to take place */
    reset_timer();
    while(delay_msec() < 20);

    /* Read operation with 2 byte request*/
    si7021_i2c_read_data(2);

    /* read temperature value */
    si7021_read_temperature();
}

/*******************************************************************************
* @Function si7021_read_humidity
* @Description: calculates the humidity value as per the datasheet specification
* @input param : none
* @return: none
*******************************************************************************/
static void si7021_read_humidity()
{
    int hum;
    float humidity;

    /* Read the required bytes as per the frame received */
    hum = ((si7021_databuffer[0] << 8) | si7021_databuffer[1]);

    /* calculate Humidity  asper datasheeet */
    humidity = hum;
    humidity = humidity*125;
    humidity = humidity/65536;
    humidity = humidity - 6;

    /* Update the global variable for SI7021 humidity */
    si7021_humidity = humidity;
}

/*******************************************************************************
* @Function si7021_read_temperature
* @Description: calculates the temperature value as per the datasheet specification
* @input param : none
* @return: none
*******************************************************************************/
static void si7021_read_temperature()
{
    int temp;
    float temperature;

    /* Read the required bytes as per the frame received */
    temp = ((si7021_databuffer[0] << 8) | si7021_databuffer[1]);

    /* calculate Temperature as per datasheet  */
    temperature = temp;
    temperature = temperature*175.72;
    temperature = temperature/65536;
    temperature = temperature - 46.85;

    /* Update the global variable for SI7021 temperature */
    si7021_temp_in_degC = temperature;
}
