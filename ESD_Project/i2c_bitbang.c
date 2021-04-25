/* -----------------------------------------------------------------------------
 * Rishab Shah
 * ECEN 5613 - Spring 2021 - Prof. McClure
 * University of Colorado Boulder
 * Revised 19/03/2021
 * references:https://aticleworld.com/interfacing-eeprom-using-i2c/
 * https://8051projects.net/wiki/I2C_Implementation_on_8051#Implementing_I2C_in_C
 The function names and implementation sequence is slightly modified after
 analysing the same from the datasheet.

 The main base logic required to develop the code was taken from her for I2C.
 *  ----------------------------------------------------------------------------
 * This files handles all the operations related to I2c.
   ---------------------------------------------------------------------------*/
#include "i2c_bitbang.h"
/* The function puts a start condition on the bus*/

#define SET_SDA_HIGH                            P1->OUT |= SDA
#define SET_SDA_LOW                             P1->OUT &= ~SDA
#define SET_SCL_HIGH                            P1->OUT |= SCL
#define SET_SCL_LOW                             P1->OUT &= ~SCL

void set_sda(uint8_t value);
void init_i2c_bitbang();

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

/* The function puts a stop condition on the bus*/
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

/* The function puts the bus in no busy state*/
void bus_no_busy_state()
{
   SET_SDA_HIGH;

   SET_SCL_HIGH;
}

/* The function provides a software delay for triggering the timing.
chosen empircally as I2C_DELAY = 1 */
void mydelay(uint8_t data)
{
    while(data)
    {
        data = data - 1;
    }
}

/* This performs a write operation on the bus for the 8 bits passed */
uint8_t am2320_i2c_write_operation_wakeup(uint8_t Txdata)
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

    return 0;
}

/* This performs a read operation on the bus for the 8 bits passed by the slave
on bus */
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

/* This is used to send an ack during sequential read */
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

/* This is used to prepare a restart sequence */
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

/* This is used to send a NACK during the end of read sequence*/
void send_nack(void)
{
    SET_SDA_HIGH;
    mydelay(I2C_DELAY);

    SET_SCL_HIGH;
    mydelay(I2C_DELAY);
}

/* This is used to poll for the ack from the slave during write opearation */
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
/*
void set_scl(uint8_t value)
{
    if(value != 0)
    {
        P1->OUT |= SCL;
    }
    else
    {
        P1->OUT &= ~SCL;
    }
}
*/
void init_i2c_bitbang()
{
    //Configure GPIO
    P1->DIR |= SDA;
    P1->DIR |= SCL;

    SEL_0 &= ~SDA;
    SEL_1 &= ~SDA;

    SEL_0 &= ~SCL;
    SEL_1 &= ~SCL;

    P1->OUT |= SDA;
    P1->OUT |= SCL;
}
