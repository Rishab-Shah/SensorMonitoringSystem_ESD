#include "msp.h"


/**
 * main.c
 */
#include "main.h"

#define POLL_FREQ_AM2320                         (2000)
#define TEMPERATURE_SWITCH                       (0)
#define I2C_DATA_READ_BYTES                      (7)
#define AM2320_BITBANG_ADDRESS                   (0xB8)

int32_t am2320_poll_frequency;

float am2320_temp_in_degC = 20.0;
float am2320_humidity = 35.0;

extern uint8_t am2320_databuffer[8];
void am2320_temparutre_humidity_measureent();
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

	am2320_poll_frequency = POLL_FREQ_AM2320;

    while(1)
    {
        if(delay_msec() > am2320_poll_frequency)
        {

#if TEMPERATURE_SWITCH

            am2320_temparutre_humidity_measureent();
            printf("am2320_poll_frequency::%d\r\n",am2320_poll_frequency);
#endif

            calculate_rtc_time(rtc_buffer);
            strcpy(displaystring,rtc_buffer);

            sprintf(temperaturebuff,"%2.2f",am2320_temp_in_degC);
            strncat(displaystring,temperaturebuff,strlen(temperaturebuff));

            strncat(displaystring,",",1);

            sprintf(humiditybuffer,"%2.2f",am2320_humidity);
            strncat(displaystring,humiditybuffer,strlen(humiditybuffer));

            strncat(displaystring,"\r\n",strlen("\r\n"));
            printf("%s",displaystring);

            strcpy(lcd_string,"Temp(*c):");
            strncat(lcd_string,temperaturebuff,strlen(temperaturebuff));
            set_address(0, 2);
            write_string_to_lcd(lcd_string);

            strcpy(lcd_string,"Hum(RH):");
            strncat(lcd_string,humiditybuffer,strlen(humiditybuffer));
            set_address(0, 3);
            write_string_to_lcd(lcd_string);
            set_address(0, 4);
            write_string_to_lcd("Battery level:");

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

}

void am2320_temparutre_humidity_measureent()
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

    am2320_i2c_write_data(FUNCTION_CODE,START_ADDRESS,REGISTER_LENGTH,3);

    reset_timer();
    while(delay_msec() < 2);

    am2320_i2c_init();
    am2320_i2c_read_data(I2C_DATA_READ_BYTES);

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

/* EOF */
