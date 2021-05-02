/******************************************************************************
* @file: i2c_bitbang.c
*
* @brief: This file was incorporated to include the bit banging technique for
* AM2320 sensor for the wakeup sequence correction
* @date:  25-Apr-2021
*******************************************************************************/
/*******************************************************************************
Header files
*******************************************************************************/
#include "i2c_bitbang.h"
/*******************************************************************************
Macros
*******************************************************************************/
#define SET_SDA_HIGH                            P1->OUT |= SDA
#define SET_SDA_LOW                             P1->OUT &= ~SDA
#define SET_SCL_HIGH                            P1->OUT |= SCL
#define SET_SCL_LOW                             P1->OUT &= ~SCL

#define WRITE                           (0)
#define READ                            (1)
#define I2C_DELAY                       (2)
/*******************************************************************************
Function Prototype
*******************************************************************************/
static void set_sda(uint8_t value);
static void init_i2c_bitbang();
/*******************************************************************************
Function Definition
*******************************************************************************/
/*******************************************************************************
* @Function start_bit
* @Description: This function generates the start bit
* @input param : None
* @return: None
*******************************************************************************/
void start_bit()
{
    SET_SDA_HIGH;
    mydelay(I2C_DELAY);

    SET_SCL_HIGH;
    mydelay(I2C_DELAY);

    /* Transition from high to low detected when SCL is high */
    SET_SDA_LOW;
    mydelay(I2C_DELAY);

    SET_SCL_LOW;
    mydelay(I2C_DELAY);
}

/*******************************************************************************
* @Function stop_bit
* @Description: The function puts a stop condition on the bus
* @input param : None
* @return: None
*******************************************************************************/
void stop_bit()
{
   SET_SCL_LOW;
   mydelay(I2C_DELAY);

   SET_SDA_LOW;
   mydelay(I2C_DELAY);

   SET_SCL_HIGH;
   mydelay(I2C_DELAY);

   /* Transition from low to high detected when SCL is high */
   SET_SDA_HIGH;
   mydelay(I2C_DELAY);

}

/*******************************************************************************
* @Function bus_no_busy_state
* @Description: The function puts the bus in no busy state
* @input param : None
* @return: None
*******************************************************************************/
void bus_no_busy_state()
{
   SET_SDA_HIGH;

   SET_SCL_HIGH;
}

/*******************************************************************************
* @Function mydelay
* @Description: The function provides a software delay for triggering the timing.
* chosen empirically as I2C_DELAY = 2
* @input param : None
* @return: None
*******************************************************************************/
void mydelay(uint8_t data)
{
    while(data)
    {
        data = data - 1;
    }
}

/*******************************************************************************
* @Function am2320_i2c_write_operation_wakeup
* @Description: This performs a write operation on the bus for the 8 bits passed
* @input param 1: teh data to be sent over the frame
* @return: none
*******************************************************************************/
void am2320_i2c_write_operation_wakeup(uint8_t Txdata)
{
    init_i2c_bitbang();

    start_bit();

    /* Ensure SCL is low before SDA sends data to make a proper read */
    SET_SCL_LOW;
    mydelay(I2C_DELAY);
    uint8_t temp;

    for(uint8_t i = 8; i > 0; i--)
    {
        /* Put data (bit) on the bus */
        temp =  (Txdata & (1 << (i-1)));
        set_sda(temp);
        mydelay(I2C_DELAY);

        /* change SCL to high to read the recent bit transition*/
        SET_SCL_HIGH;
        mydelay(I2C_DELAY);

        /*Prepare bus for next bit change*/
        SET_SCL_LOW;
        mydelay(I2C_DELAY);
    }

    /* generate NACK and pull SDA low */
    SET_SDA_HIGH;
    mydelay(I2C_DELAY);

    SET_SCL_HIGH;
    mydelay(I2C_DELAY);

    SET_SCL_LOW;
    mydelay(I2C_DELAY);

    SET_SDA_LOW;
    mydelay(I2C_DELAY);

//-----------------------------------------------------------
    reset_timer();
    while(delay_msec() < 2);

    stop_bit();
}

/*******************************************************************************
* @Function i2c_read_operation
* @Description: This performs a read operation on the bus for the 8 bits passed by the slave
* on bus
* @input param : none
* @return: the byte received on the frame
*******************************************************************************/
#if 0
unsigned char i2c_read_operation()
{
    uint8_t Data1 = 0;
    /* Ensure SCL is low before SDA sends data to make a proper read */
    SET_SCL_LOW;
    mydelay(I2C_DELAY);

    for(uint8_t i = 8; i > 0; i--)
    {
        /* read data (bit) on the bus */
        Data1 |= (SDA << (i-1));
        mydelay(I2C_DELAY);

        /* change SCL to high to read the recent bit transition*/
        SET_SCL_HIGH;
        mydelay(I2C_DELAY);

        /*Prepare bus for next bit change*/
        SET_SCL_LOW;
        mydelay(I2C_DELAY);
    }

    return Data1;
}
#endif

/*******************************************************************************
* @Function SendACK
* @Description: This is used to send an ack during sequential read
* @input param : none
* @return: none
*******************************************************************************/
void SendACK()
{
    SET_SDA_LOW;
    mydelay(I2C_DELAY);

    SET_SCL_HIGH;
    mydelay(I2C_DELAY);

    SET_SCL_LOW;
    mydelay(I2C_DELAY);

    SET_SDA_HIGH;
    mydelay(I2C_DELAY);
}

/*******************************************************************************
* @Function restart_bit
* @Description: This is used to prepare a restart sequence
* @input param : none
* @return: none
*******************************************************************************/
void restart_bit()
{
    SET_SDA_HIGH;
    mydelay(I2C_DELAY);

    SET_SCL_HIGH;
    mydelay(I2C_DELAY);

    SET_SDA_LOW;
    mydelay(I2C_DELAY);

    SET_SCL_LOW;
    mydelay(I2C_DELAY);
}

/*******************************************************************************
* @Function send_nack
* @Description: This is used to send a NACK during the end of read sequence
* @input param : none
* @return: none
*******************************************************************************/
void send_nack(void)
{
    SET_SDA_HIGH;
    mydelay(I2C_DELAY);

    SET_SCL_HIGH;
    mydelay(I2C_DELAY);
}

/*******************************************************************************
* @Function poll_ack
* @Description: This is used to poll for the ack from the slave during write operation
* @input param : none
* @return: none
*******************************************************************************/
void poll_ack()
{
    SET_SCL_HIGH;
    mydelay(I2C_DELAY);

    /*while(SDA == HIGH)
    {
        ;
    }*/

    SET_SCL_LOW;
    mydelay(I2C_DELAY);
}

/*******************************************************************************
* @Function set_sda
* @Description: This is used to send the 1 or 0 data on the frame bit by bit
* @input param 1: the value received
* @return: none
*******************************************************************************/
void set_sda(uint8_t value)
{
    if(value != 0)
    {
        P1->OUT |= SDA;
    }
    else
    {
        P1->OUT &= ~SDA;
    }
}

/*******************************************************************************
* @Function init_i2c_bitbang
* @Description: Initialize the GPIO pins for the I2C bit bang arrangement
* @input param : none
* @return: none
*******************************************************************************/
void init_i2c_bitbang()
{
    /* Configure GPIO pins for
     * bitbang arrangement */
    P1->DIR |= SDA;
    P1->DIR |= SCL;

    /* Using P1.6 and P1.7
     * for the same */
    SEL_0 &= ~SDA;
    SEL_1 &= ~SDA;

    SEL_0 &= ~SCL;
    SEL_1 &= ~SCL;

    /* By default put them in I2C
     * No busy state */
    P1->OUT |= SDA;
    P1->OUT |= SCL;
}


/* EOF */
