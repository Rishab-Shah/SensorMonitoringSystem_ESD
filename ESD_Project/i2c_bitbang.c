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

void set_sda(uint8_t value);
void set_scl(uint8_t value);
void init_i2c_bitbang();
void start_bit()
{
   //putstr("START BIT\r\n");

   //SDA = HIGH;

   //P1->OUT |= SDA;
   //for(int i=0;i<2;i++);
   set_sda(HIGH);
  // mydelay(I2C_DELAY);

   //SCL = HIGH;

   //P1->OUT |= SCL;
   //for(int i=0;i<2;i++);
   set_scl(HIGH);
  // mydelay(I2C_DELAY);

   /* Transition from high to low detected when SCL is high */
   //SDA = LOW;

   //P1->OUT &= ~SDA;
   //for(int i=0;i<2;i++);
   set_sda(LOW);
  // mydelay(I2C_DELAY);

   //SCL = LOW;

   //P1->OUT &= ~SCL;
   //for(int i=0;i<2;i++);
   set_scl(LOW);
  // mydelay(I2C_DELAY);

}

/* The function puts a stop condition on the bus*/
void stop_bit()
{
   set_scl(LOW);
   mydelay(I2C_DELAY);

   set_sda(LOW);
   mydelay(I2C_DELAY);

   set_scl(HIGH);
   mydelay(I2C_DELAY);

   /* Transition from low to high detected when SCL is high */
   set_sda(HIGH);
   mydelay(I2C_DELAY);

}

/* The function puts the bus in no busy state*/
void bus_no_busy_state()
{
   set_sda(HIGH);

   set_scl(HIGH);
}

/* The function initalised the bus */
/*
void i2c_init()
{
   SDA = LOW;

   SCL = LOW;
}
*/
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
uint8_t i2c_write_operation_wakeup(uint8_t Txdata)
{

    init_i2c_bitbang();

    set_sda(HIGH);
    mydelay(I2C_DELAY);

    set_scl(HIGH);
    mydelay(I2C_DELAY);

    set_sda(LOW);
    mydelay(I2C_DELAY);

    set_scl(LOW);
    mydelay(I2C_DELAY);

//------------------------------------------------------------------//

    /* Ensure SCL is low before SDA sends data to make a proper read */
    set_scl(LOW);
    mydelay(I2C_DELAY);
    uint8_t temp;

    for(uint8_t i = 8; i > 0; i--)
    {
        /* Put data (bit) on the bus */
        temp =  (Txdata & (1 << (i-1)));
        set_sda(temp);
        mydelay(I2C_DELAY);

        /* change SCL to high to read the recent bit transition*/
        set_scl(HIGH);
        mydelay(I2C_DELAY);

        /*Prepare bus for next bit change*/
        set_scl(LOW);
        mydelay(I2C_DELAY);
    }

    set_sda(HIGH);
    mydelay(I2C_DELAY);

    set_scl(HIGH);
    mydelay(I2C_DELAY);

    set_scl(LOW);
    mydelay(I2C_DELAY);

    set_sda(LOW);
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
    //SCL = LOW;
    set_scl(LOW);
    mydelay(I2C_DELAY);

    for(uint8_t i = 8; i > 0; i--)
    {
        /* read data (bit) on the bus */
        Data1 |= (SDA << (i-1));
        mydelay(I2C_DELAY);

        /* change SCL to high to read the recent bit transition*/
        //SCL = HIGH;
        set_scl(HIGH);
        mydelay(I2C_DELAY);

        /*Prepare bus for next bit change*/
        //SCL = LOW;
        set_scl(LOW);
        mydelay(I2C_DELAY);
    }

    return Data1;
}
#endif

/* This is used to send an ack during sequential read */
void SendACK()
{

    set_sda(LOW);
    mydelay(I2C_DELAY);


    set_scl(HIGH);
    mydelay(I2C_DELAY);


    set_scl(LOW);
    mydelay(I2C_DELAY);


    set_sda(HIGH);
    mydelay(I2C_DELAY);
}

/* This is used to prepare a restart sequence */
void restart_bit()
{

    set_sda(HIGH);


    set_scl(HIGH);


    set_sda(LOW);

    set_scl(LOW);
}

/* This is used to send a NACK during the end of read sequence*/
void SendNACK(void)
{
    set_sda(HIGH);
    mydelay(I2C_DELAY);

    set_scl(HIGH);
    mydelay(I2C_DELAY);
}

/* This is used to poll for the ack from the slave during write opearation */
void PollAck()
{
    set_scl(HIGH);

    while(SDA == HIGH)
    {
        ;
    }

    set_scl(LOW);
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


#if 0
/* This is used to write the data into eeprom in a particular memory location */
uint8_t eebytew(uint8_t blocknumber, uint8_t addr,uint8_t databyte)
{
    start_bit();
    i2c_write_operation(blocknumber);
    i2c_write_operation(addr);
    i2c_write_operation(databyte);
    stop_bit();
    return 0;
}

/* This is used to read the data from the eeprom  from a particular memory location
The implementation is for random read */
uint8_t eebyter(uint8_t blocknumber,uint8_t addr,uint8_t blocknumberread)
{
    uint8_t datastore;
    start_bit();
    i2c_write_operation(blocknumber);       //control byte - write
    i2c_write_operation(addr);              //word address
    start_bit();
    i2c_write_operation(blocknumberread);   //control byte
    datastore = i2c_read_operation();       //read data
    SendNACK();
    stop_bit();
    return datastore;
}

#endif


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
