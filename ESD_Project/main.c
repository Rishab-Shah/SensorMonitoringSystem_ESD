#include "msp.h"


/**
 * main.c
 */
#include "main.h"

#define T_RH_POLL_FREQ                           (1000)
#define TEMPERATURE_AM2320                       (0)
#define HEART_BEAT                               P1->OUT ^= BIT0
#define TEMP_THRESHOLD                           23

int32_t T_RH_poll_frequency;

volatile float am2320_temp_in_degC;
volatile float am2320_humidity;
volatile float si7021_temp_in_degC;
volatile float si7021_humidity;

static void init_routine();
static void sensor_processing();
static void control_action();
static void display_dewpoint_level(float temperature, float relative_humidity);

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	init_routine();

	T_RH_poll_frequency = T_RH_POLL_FREQ;

    while(1)
    {
        if(delay_msec() > T_RH_poll_frequency)
        {
            sensor_processing();

            control_action();

            HEART_BEAT;
            reset_timer();
        }

        __sleep();
        __no_operation();

    }

}


static void init_routine()
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

static void sensor_processing()
{
    char displaystring[200];
    char rtc_buffer[25];
    char temperaturebuff[10];
    char humiditybuffer[10];
    char lcd_string[50];

#if TEMPERATURE_AM2320

    am2320_temparutre_humidity_measurement();

    am2320_display_builder();

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

    display_dewpoint_level(si7021_temp_in_degC,si7021_humidity);

    cbfifo_enqueue(&cbfifo_tx,displaystring,strlen(displaystring));
}


static void display_dewpoint_level(float temperature_c, float relative_humidity)
{
    set_address(0, 4);

    char dew_point_buffer[25];
    char lcd_string[50];

    float dew_pt = temperature_c - ((100 - relative_humidity)/5);

    sprintf(dew_point_buffer,"%2.2f",dew_pt);
    strcpy(lcd_string,"Dew Pt :");
    strncat(lcd_string,dew_point_buffer,strlen(dew_point_buffer));

    write_string_to_lcd(lcd_string);
}


static void control_action()
{
    if(si7021_temp_in_degC > TEMP_THRESHOLD)
    {
        TURN_FAN_ON;
    }
    else
    {
        TURN_FAN_OFF;
    }
}
/* EOF */
