#include "msp.h"


/**
 * main.c
 */
#include "main.h"

#define POLL_FREQ_AM2320                3000
#define TEMPERATURE                     1


int32_t am2320_poll_frequency;
extern uint8_t data_buffer[8];
void temparutremeasureent();
void init_routine();

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	init_routine();

	am2320_poll_frequency = POLL_FREQ_AM2320;

	//Configure GPIO
    P1->DIR |= BIT0;
    P1->OUT &= ~BIT0;

    while(1)
    {
        if(delay_msec() > am2320_poll_frequency)
        {
#if TEMPERATURE
            temparutremeasureent();
            printf("am2320_poll_frequency::%d\r\n",am2320_poll_frequency);
#endif

            P1->OUT ^= BIT0;
            reset_timer();
        }

        __sleep();
        __no_operation();
    }
}


void init_routine()
{
    init_timer();
#if TEMPERATURE
    i2c_init();

    switch_init();
    switch_interrupt_init();
#endif
}

void temparutremeasureent()
{
    signed int T = 0x0000;
    unsigned int RH = 0x0000;
    unsigned int CRC_data = 0x0000;
    unsigned int CRC_temp = 0x0000;

    i2c_write_wakeup();

    reset_timer();
    while(delay_msec() < 10);


    i2c_write_data(FUNCTION_CODE,START_ADDRESS,REGISTER_LENGTH,3);

    reset_timer();
    while(delay_msec() < 2);

    i2c_read_data(7);

    get_RH_and_temperature(&RH, &T);
    get_CRC(&CRC_temp);
    CRC_data = CRC16(data_buffer, 6);

    if(CRC_temp == CRC_data)
    {
      printf("RH::%x\n",RH/10);
      printf("T::%x\n",T/10);
      //lcd_print(8, 2, RH);
      //lcd_print(8, 3, T);
      //WordToHex(CRC_data, txt);
      //lcd_out(4, 8, "0x");
      //lcd_out(4, 10, txt);
    }
    else
    {
      printf("error\n");
    }

    reset_timer();
    while(delay_msec() < 10);
}
