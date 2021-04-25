#include "msp.h"


/**
 * main.c
 */
#include "main.h"

#define T_RH_POLL_FREQ                           (2000)
#define TEMPERATURE_AM2320                       (0)
#define HEART_BEAT

int32_t T_RH_poll_frequency;

volatile float am2320_temp_in_degC;
volatile float am2320_humidity;
volatile float si7021_temp_in_degC;
volatile float si7021_humidity;

extern uint8_t am2320_databuffer[8];
void am2320_temparutre_humidity_measurement();
void init_routine();

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	init_routine();

	char displaystring[200];
	char rtc_buffer[25];
	char temperaturebuff[10];
	char humiditybuffer[10];
	char lcd_string[50];

	T_RH_poll_frequency = T_RH_POLL_FREQ;

    while(1)
    {
        if(delay_msec() > T_RH_poll_frequency)
        {

#if TEMPERATURE_AM2320

            am2320_temparutre_humidity_measurement();

            calculate_rtc_time(rtc_buffer);
            strcpy(displaystring,rtc_buffer);

            sprintf(temperaturebuff,"%2.2f",am2320_temp_in_degC);
            strncat(displaystring,temperaturebuff,strlen(temperaturebuff));

            strncat(displaystring,",",1);

            sprintf(humiditybuffer,"%2.2f",am2320_humidity);
            strncat(displaystring,humiditybuffer,strlen(humiditybuffer));

#else
            si7021_temperature_humidity_measurement();
            calculate_rtc_time(rtc_buffer);
            strcpy(displaystring,rtc_buffer);

            sprintf(temperaturebuff,"%2.2f",si7021_temp_in_degC);
            strncat(displaystring,temperaturebuff,strlen(temperaturebuff));

            strncat(displaystring,",",1);

            sprintf(humiditybuffer,"%2.2f",si7021_humidity);
            strncat(displaystring,humiditybuffer,strlen(humiditybuffer));
#endif
            strncat(displaystring,"\r\n",strlen("\r\n"));

            /* Display on the LCD module */
            strcpy(lcd_string,"Temp(c):");
            strncat(lcd_string,temperaturebuff,strlen(temperaturebuff));
            set_address(0, 2);
            write_string_to_lcd(lcd_string);

            strcpy(lcd_string,"Hum(RH):");
            strncat(lcd_string,humiditybuffer,strlen(humiditybuffer));
            strncat(lcd_string,"%",strlen("%"));
            set_address(0, 3);
            write_string_to_lcd(lcd_string);
            set_address(0, 4);
//            //write_string_to_lcd("Battery level:");



            cbfifo_enqueue(&cbfifo_tx,displaystring,strlen(displaystring));

            P1->OUT ^= BIT0;
            reset_timer();
        }

        __sleep();
        __no_operation();

    }

}


void init_routine()
{
    clock_init();

    init_timer();

    rtc_init();

    config_uart();

    switch_init();

    gpio_init();

    spi_config();

    init_lcd();

    si7021_i2c_init();

    reset_timer();
    while(delay_msec() < 5);

}

#if 0
void am2320_temparutre_humidity_measurement()
{
    signed int am2320_T = 0x0000;
    unsigned int am2320_RH = 0x0000;
    unsigned int am2320_CRC_data = 0x0000;
    unsigned int am2320_CRC_temp = 0x0000;

    am2320_i2c_write_operation_wakeup(AM2320_BITBANG_ADDRESS);

    reset_timer();
    while(delay_msec() < 5);

    am2320_i2c_init();

    reset_timer();
    while(delay_msec() < 5);

    am2320_i2c_write_data(AM2320_FUNCTION_CODE,AM2320_START_ADDRESS,AM2320_REGISTER_LENGTH,3);

    reset_timer();
    while(delay_msec() < 2);

    am2320_i2c_init();
    am2320_i2c_read_data(AM2320_I2C_DATA_READ_BYTES);

    am2320_get_RH_and_temperature(&am2320_RH, &am2320_T);
    am2320_get_CRC(&am2320_CRC_temp);
    am2320_CRC_data = am2320_CRC16(am2320_databuffer, 6);

    if(am2320_CRC_temp == am2320_CRC_data)
    {
        am2320_temp_in_degC = (((am2320_databuffer[4] & 0x7F) << 8) + am2320_databuffer[5]) / 10.0;
        am2320_temp_in_degC = (am2320_databuffer[4] & 0x80) ? -am2320_temp_in_degC : am2320_temp_in_degC;
        am2320_humidity = ((am2320_databuffer[2] << 8) + am2320_databuffer[3]) / 10.0;

        printf("RH::%x\n",am2320_RH/10);
        printf("T::%x\n",am2320_T/10);
        printf("New data\n");
        printf("RH::%f\n",am2320_temp_in_degC);
        printf("T::%f\n",am2320_humidity);

    }
    else
    {
        printf("error\n");
    }

    reset_timer();
    while(delay_msec() < 10);
}
#endif

/* EOF */
