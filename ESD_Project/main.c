/******************************************************************************
* @file: main.c
*
* @brief: This files consists of the function definitions
* This file is the main file. All the initialization is done here and the
* application entry point code is written here
* @author: Rishab Shah
* @date:  10-Apr-2021
*******************************************************************************/
/*******************************************************************************
Header files
*******************************************************************************/
#include "main.h"
/*******************************************************************************
Macros
*******************************************************************************/
#define TEMPERATURE_AM2320                        (0)
#define HEART_BEAT                                P1->OUT ^= BIT0
#define TEMP_THRESHOLD                            23        /* For testing 23 degree celsius is chosen */
/*******************************************************************************
Global Variables
*******************************************************************************/
int32_t T_RH_poll_frequency;
volatile float am2320_temp_in_degC;
volatile float am2320_humidity;
volatile float si7021_temp_in_degC;
volatile float si7021_humidity;
/*******************************************************************************
Function Prototype
*******************************************************************************/
static void init_routine();
static void sensor_processing();
static void control_action();
static void display_dewpoint_level(float temperature, float relative_humidity);
static void disable_watchdog();
/*******************************************************************************
Function definitions
*******************************************************************************/
/*******************************************************************************
* @Function main
* @Description: Entry point of the code
* @input param : None
* @return: None
*******************************************************************************/
void main(void)
{
    disable_watchdog();

    /* Initialize all the modules */
	init_routine();

	/* Set the default polling frequency */
	T_RH_poll_frequency = T_RH_BASE_POLL_FREQ;

    while(1)
    {
        /* Trigger LED module at the frequency decided */
        if(delay_msec() > T_RH_poll_frequency)
        {
            /* Sense the temperature and Humidity data from
             * the AM2320,SI7021 module and transmit it to the LCD
             * and host PC */
            sensor_processing();

            /* Take any control action if required
             * based upon temprature and humidity value  */
            control_action();

            /* Indicates that the system is alive
             * and is functioning well to the user */
            HEART_BEAT;

            /*Reset the timer so that the system is free
             * to other activities or go to sleep and when
             * the wakeup time happens do the required
             * processing  */
            reset_timer();
        }

        /* go to sleep as ther is no
         * other work */
        __sleep();
        __no_operation();

    }
}

/*******************************************************************************
* @Function init_routine
* @Description: Initializes all the modules in the application software
* @input param : None
* @return: None
*******************************************************************************/
static void init_routine()
{
    /* initialise the system clck to operate at 12Mhz */
    clock_init();

    /* initialise the systick timer */
    init_timer();

    /* initialise the RTC module for timestamp */
    rtc_init();

    /* Configures the UART for communication
     * with the host PC */
    config_uart();

    /* Configured to vary the sensor polling frequency */
    switch_init();

    /* Configure the GPIO pin for Hear beat LED indication  */
    gpio_init();

    /* Configure the GPIO pin for Hear beat LED indication  */
    spi_config();

    /* Configure the LCD module at initialization stage  */
    init_lcd();

    /* Configure the I2C module for SI7021 sensor  */
    si7021_i2c_init();

    /* A delay for the LCD initialization processing to complete
     * and sensor to achieve it;s full voltage levels
     * Wait before processing is started  */
    reset_timer();
    while(delay_msec() < 5);

}

/*******************************************************************************
* @Function sensor_processing
* @Description: Senses all the data as per teh polling rate determined by the
* on-board switches or the default polling frequency as per T_RH_BASE_POLL_FREQ
* @input param : None
* @return: None
*******************************************************************************/
static void sensor_processing()
{
    char displaystring[200];
    char rtc_buffer[25];
    char temperaturebuff[10];
    char humiditybuffer[10];
    char lcd_string[50];

#if TEMPERATURE_AM2320

    /* The AM2320 sensor was not running reliable.
     * The software is fine.
     * The frames match exactly with the datasheet
     * The timing analysis of arduino code and
     * the datasheet are also in sync
     * Since, the code was not running reliablt
     * Moved to SI7021 sensor
     */
    am2320_temparutre_humidity_measurement();

    calculate_rtc_time(rtc_buffer);
    strcpy(displaystring,rtc_buffer);

    /* Append temperature */
    sprintf(temperaturebuff,"%2.2f",am2320_temp_in_degC);
    strncat(displaystring,temperaturebuff,strlen(temperaturebuff));

    strncat(displaystring,",",1);

    /* Append Humidity */
    sprintf(humiditybuffer,"%2.2f",am2320_humidity);
    strncat(displaystring,humiditybuffer,strlen(humiditybuffer));

#else

    si7021_temperature_humidity_measurement();
    calculate_rtc_time(rtc_buffer);
    strcpy(displaystring,rtc_buffer);

    /* Append temperature */
    sprintf(temperaturebuff,"%2.2f",si7021_temp_in_degC);
    strncat(displaystring,temperaturebuff,strlen(temperaturebuff));

    strncat(displaystring,",",1);

    /* Append Humidity */
    sprintf(humiditybuffer,"%2.2f",si7021_humidity);
    strncat(displaystring,humiditybuffer,strlen(humiditybuffer));

#endif

    strncat(displaystring,"\r\n",strlen("\r\n"));

    /* send the data over UART to PC
     * Format::
     * "DD:MM:YY:HH:MM:SS::temp,Hum\r\n */
    cbfifo_enqueue(&cbfifo_tx,displaystring,strlen(displaystring));

    /* Display on the LCD module  - temperature and it's readings  */
    strcpy(lcd_string,"Temp(c):");
    strncat(lcd_string,temperaturebuff,strlen(temperaturebuff));
    /* set the appropriate row number in the LCD module */
    set_address(0, 2);
    write_string_to_lcd(lcd_string);

    /* Display on the LCD module  - humidity and it's readings  */
    strcpy(lcd_string,"Hum(RH):");
    strncat(lcd_string,humiditybuffer,strlen(humiditybuffer));
    strncat(lcd_string,"%",strlen("%"));
    /* set the appropriate row number in the LCD module */
    set_address(0, 3);
    write_string_to_lcd(lcd_string);

    /* Display on the LCD module  - Dew point and the readings  */
    display_dewpoint_level(si7021_temp_in_degC,si7021_humidity);

}

/*******************************************************************************
* @Function display_dewpoint_level
* @Description: Calculates the dew-point based upon the temperature and humidity value.
* It is just an approximated value.
* @input param 1: temperature
* @input param 2: humidity
* @return: None
*******************************************************************************/
static void display_dewpoint_level(float temperature_c, float relative_humidity)
{
    /* set the appropriate row number in the LCD module */
    set_address(0, 4);

    char dew_point_buffer[25];
    char lcd_string[50];

    float dew_pt = temperature_c - ((100 - relative_humidity)/5);

    sprintf(dew_point_buffer,"%2.2f",dew_pt);
    strcpy(lcd_string,"Dew Pt :");
    strncat(lcd_string,dew_point_buffer,strlen(dew_point_buffer));

    write_string_to_lcd(lcd_string);
}

/*******************************************************************************
* @Function control_action
* @Description: After the data is sensed , a control action has to be taken if
* required for the same.
* @input param 1: temperature
* @input param 2: humidity
* @return: None
*******************************************************************************/
static void control_action()
{
    /* check if the temperature has crossed a
     * particular threshold */
    if(si7021_temp_in_degC > TEMP_THRESHOLD)
    {
        /* turn the Fan ON
         * indicated by Blue LED */
        TURN_FAN_ON;
    }
    else
    {
        /* turn the Fan OFF
         * indicated by Blue LED
         * turning OFF*/
        TURN_FAN_OFF;
    }
}

/*******************************************************************************
* @Function disable_watchdog
* @Description: Disables the watchdog
* @input param : none
* @return: None
*******************************************************************************/
static void disable_watchdog()
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
}

/* EOF */
