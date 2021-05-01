/******************************************************************************
* @file: lcd_spi.c
*
* @brief: This files consists of the function definitions used in the lcd_spi.c
* file
* This file is is used for communication with the LCD module. The reference code
* was for MSP430. The code was ported for the MSP432P401R
* @date:  20-Apr-2021
* @reference: William Goh - msp432p401x_euscib0_spi_09
*******************************************************************************/
/*******************************************************************************
Header files
*******************************************************************************/
#include "lcd_spi.h"
/*******************************************************************************
Macros
*******************************************************************************/
#define LCD5110_SCLK_PIN                        BIT5
#define LCD5110_DN_PIN                          BIT6

#define LCD5110_SCE_PIN                         BIT6
#define LCD5110_CHIP_SELECT                     P2->OUT &= ~LCD5110_SCE_PIN
#define LCD5110_CHIP_DESELECT                   P2->OUT |= LCD5110_SCE_PIN

#define LCD5110_DC_PIN                          BIT4
#define LCD5110_SET_COMMAND                     P2->OUT &= ~LCD5110_DC_PIN
#define LCD5110_SET_DATA                        P2->OUT |= LCD5110_DC_PIN

#define LCD_RST_PIN                             BIT7
#define LCD5110_RST_LOW                         P2->OUT &= ~LCD_RST_PIN
#define LCD5110_RST_HIGH                        P2->OUT |= LCD_RST_PIN

#define LCD5110_COMMAND                         0
#define LCD5110_DATA                            1

#define EUSCI_SPI_MSB_FIRST                     EUSCI_B2->CTLW0 |= EUSCI_B_CTLW0_MSB
#define EUSCI_SPI_LSB_FIRST                     EUSCI_B2->CTLW0 &= ~EUSCI_B_CTLW0_MSB

#define SPI_TXBUF                               EUSCI_B2->TXBUF

#define LCD_PIN_SPI_SEL0                        P3->SEL0
#define LCD_PIN_SPI_SEL1                        P3->SEL1

#define LCD_PIN_GPIO_SEL0                       P2->SEL0
#define LCD_PIN_GPIO_SEL1                       P2->SEL1

#define EUSCI_SPI_CTLW0                         EUSCI_B2->CTLW0
#define EUSCI_SPI_BRW                           EUSCI_B2->BRW
/*******************************************************************************
Function Prototype
*******************************************************************************/
static void spi_gpio_init();
/*******************************************************************************
* @Function spi_gpio_init
* @Description: This module is used to initialize the Data/Command,CS and RESET
* pins of the MSP connected to the LCD module
* @input param : None
* @return: None
*******************************************************************************/
static void spi_gpio_init()
{
    /* CS DC and RST gpio configuration */
    P2->DIR |= LCD5110_DC_PIN;      /* set as output */

    P2->DIR |= LCD5110_SCE_PIN;     /* set as output */

    P2->DIR |= LCD_RST_PIN;         /* set as output */

    /* init the LCD with RESET pin high to later create a
     * low to high transition  */
    LCD5110_RST_HIGH;

    /* SCLK and MOSI (COPI) selection */
    LCD_PIN_SPI_SEL0 |= LCD5110_SCLK_PIN | LCD5110_DN_PIN;

    LCD_PIN_SPI_SEL1 &= ~LCD5110_SCLK_PIN;

    LCD_PIN_SPI_SEL1 &= ~LCD5110_DN_PIN;
}

/*******************************************************************************
* @Function spi_config
* @Description: This module initializes teh SPI register related parameters
* @input param : None
* @return: None
*******************************************************************************/
void spi_config()
{
    /* init the CS,D/C and RESET pins gpio pins */
    spi_gpio_init();

    /* Put eUSCI state machine in reset */
    EUSCI_SPI_CTLW0 |= EUSCI_B_CTLW0_SWRST;

    /*
     Remain eUSCI state machine in reset
     Set as SPI master
     Set as synchronous mode
     Set clock polarity high
     MSB first
     */
    EUSCI_SPI_CTLW0 = EUSCI_B_CTLW0_SWRST |
                EUSCI_B_CTLW0_MST |
                EUSCI_B_CTLW0_SYNC |
                EUSCI_B_CTLW0_CKPH |
                EUSCI_B_CTLW0_MSB;

    /* 3-pin SPI (default) */
    EUSCI_SPI_CTLW0 &= ~EUSCI_B_CTLW0_MODE_0;

    /* SMCLK (12Mhz) */
    EUSCI_SPI_CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK;

    /* 12 Mhz / 12 = 1 Mhz*/
    EUSCI_SPI_BRW = 12;

    /* Start the SPI state machine */
    EUSCI_SPI_CTLW0 &= ~EUSCI_B_CTLW0_SWRST;

    /* calculated based upon 12Mhz frequency */
    __delay_cycles(500000);

}

/*******************************************************************************
* @Function init_lcd
* @Description: This module initializes the LCD module
* @input param : None
* @return: None
* @reference: http://rohitg.in/2014/11/09/Nokia5510-lcd-with-MSP430/
*******************************************************************************/
void init_lcd()
{
    /* Providing a Reset sequence to
     * initialize the LCD  properly
     * as per data sheet */
    LCD5110_RST_LOW;
    delay_usec(50000);
    LCD5110_RST_HIGH;

    /* The initialization sequence which is obtained from the datasheet */
    write_to_lcd(LCD5110_COMMAND, PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION);
    write_to_lcd(LCD5110_COMMAND, PCD8544_SETVOP | 0x3F);
    write_to_lcd(LCD5110_COMMAND, PCD8544_SETTEMP | 0x02);
    write_to_lcd(LCD5110_COMMAND, PCD8544_SETBIAS | 0x03);
    write_to_lcd(LCD5110_COMMAND, PCD8544_FUNCTIONSET);
    write_to_lcd(LCD5110_COMMAND, PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);

    clear_lcd();

    set_address(0, 0);
    write_string_to_lcd("Sensor Monitor   System");
}

/*******************************************************************************
* @Function write_to_lcd
* @Description: This module writes to the LCD module the data received from
* other modules
* @input param 1: data mode or command mode
* @input param 2: the char received
* @return: None
* @reference: http://rohitg.in/2014/11/09/Nokia5510-lcd-with-MSP430/
*******************************************************************************/
void write_to_lcd(uint8_t command, uint8_t data)
{
    /* select cs to be low */
    LCD5110_CHIP_SELECT;

    /* delay is required for the data to
     * be detected properly */
    delay_usec(2);   //8 usec delay for 12Mhz clock

    if(command)
    {
        LCD5110_SET_DATA;
    }
    else
    {
        LCD5110_SET_COMMAND;
    }

    /* write data into the transmit buffer */
    SPI_TXBUF = data;

    /* wait for the RxBUf to become empty */
    while(!(EUSCI_B2->IFG & EUSCI_B_IFG_TXIFG));

    /* Wait for SPI to complete the internalopertion */
    while(EUSCI_B2->STATW & EUSCI_B_STATW_BBUSY);

    /* Give breather space for the internal operation
     * to be completed */
    delay_usec(3);
    LCD5110_CHIP_DESELECT;
}

/*******************************************************************************
* @Function write_char_to_lcd
* @Description: This module sends the character one by one to the LCD
* @input param 1: data mode or command mode
* @return: None
* @reference: http://rohitg.in/2014/11/09/Nokia5510-lcd-with-MSP430/
*******************************************************************************/
void write_char_to_lcd(uint8_t ch)
{
    uint8_t i = 0;

    for(i = 0; i < 5; i++)
    {
        write_to_lcd(LCD5110_DATA, font[ch - 0x20][i]);
    }

    write_to_lcd(LCD5110_DATA, 0);
}

/*******************************************************************************
* @Function write_string_to_lcd
* @Description: This module sends the char one till nul is reached to the LCD module
* @input param 1: data mode or command mode
* @return: None
* @reference: http://rohitg.in/2014/11/09/Nokia5510-lcd-with-MSP430/
*******************************************************************************/
void write_string_to_lcd(const char *string)
{
    while(*string)
    {
        write_char_to_lcd(*string++);
    }
}

/*******************************************************************************
* @Function clear_lcd
* @Description: This module clears all the banks of the LCD
* @return: None
* @reference: http://rohitg.in/2014/11/09/Nokia5510-lcd-with-MSP430/
*******************************************************************************/
void clear_lcd()
{
   set_address(0, 0);

   int i = 0;

   while(i < PCD8544_MAXBYTES)
   {
       /* clear the LCD screen */
       write_to_lcd(LCD5110_DATA, 0);
       i++;
   }

   /* Bring the address back to row 0 col 0 */
   set_address(0, 0);
}

/*******************************************************************************
* @Function set_address
* @Description: This module sets the address at which teh text is to be written
* @return: None
* @reference: http://rohitg.in/2014/11/09/Nokia5510-lcd-with-MSP430/
*******************************************************************************/
void set_address(uint8_t xaddress, uint8_t yaddress)
{
    write_to_lcd(LCD5110_COMMAND, PCD8544_SETXADDR | xaddress);
    write_to_lcd(LCD5110_COMMAND, PCD8544_SETYADDR | yaddress);
}

/* EOF */
