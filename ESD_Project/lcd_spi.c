/******************************************************************************
* @file:switch.c
*
* @brief: This files consists of the function headers used in the switch.c file
*
* @author: Rishab Shah
* @date:  12-Mar-2021
*******************************************************************************/
/*******************************************************************************
Header files
*******************************************************************************/
#include <lcd_spi.h>
/*******************************************************************************
Global variables
*******************************************************************************/

/*******************************************************************************
Function definition
*******************************************************************************/
#define LCD5110_SCLK_PIN                        BIT5
#define LCD5110_DN_PIN                          BIT6

#define LCD5110_SCE_PIN                         BIT6
#define LCD5110_CHIP_SELECT                     P2->OUT &= ~LCD5110_SCE_PIN
#define LCD5110_CHIP_DESELECT                   P2->OUT |= LCD5110_SCE_PIN

#define LCD5110_DC_PIN                          BIT4
#define LCD5110_SET_COMMAND                     P2->OUT &= ~LCD5110_DC_PIN
#define LCD5110_SET_DATA                        P2->OUT |= LCD5110_DC_PIN

#define LCD5110_COMMAND                         0
#define LCD5110_DATA                            1

#define LCD_RST_PIN                             BIT7
#define LCD5110_RST_LOW                         P2->OUT &= ~LCD_RST_PIN
#define LCD5110_RST_HIGH                        P2->OUT |= LCD_RST_PIN

#define EUSCI_SPI_MSB_FIRST                     EUSCI_B2->CTLW0 |= EUSCI_B_CTLW0_MSB
#define EUSCI_SPI_LSB_FIRST                     EUSCI_B2->CTLW0 &= ~EUSCI_B_CTLW0_MSB

#define SPI_TXBUF                               EUSCI_B2->TXBUF

#define LCD_PIN_SPI_SEL0                        P3->SEL0
#define LCD_PIN_SPI_SEL1                        P3->SEL1

#define LCD_PIN_GPIO_SEL0                       P2->SEL0
#define LCD_PIN_GPIO_SEL1                       P2->SEL1

#define EUSCI_SPI_CTLW0                         EUSCI_B2->CTLW0
#define EUSCI_SPI_BRW                           EUSCI_B2->BRW


static void spi_gpio_init();



/*******************************************************************************
* @Function spi_init
* @Description:
* @input param : None
* @return: None
*******************************************************************************/
static void spi_gpio_init()
{
    /* CS DC and RST gpio configuration */
    P2->DIR |= LCD5110_DC_PIN;      /* set as output */

    P2->DIR |= LCD5110_SCE_PIN;     /* set as output */

    P2->DIR |= LCD_RST_PIN;         /* set as output */

    LCD5110_RST_HIGH;

    /* SCLK and MOSI selection */
    LCD_PIN_SPI_SEL0 |= LCD5110_SCLK_PIN | LCD5110_DN_PIN;                // Set P3.5, 3.6

    LCD_PIN_SPI_SEL1 &= ~LCD5110_SCLK_PIN;

    LCD_PIN_SPI_SEL1 &= ~LCD5110_DN_PIN;
}

void spi_config()
{
    spi_gpio_init();

    EUSCI_SPI_CTLW0 |= EUSCI_B_CTLW0_SWRST;          // Put eUSCI state machine in reset

    EUSCI_SPI_CTLW0 = EUSCI_B_CTLW0_SWRST |          // Remain eUSCI state machine in reset
                EUSCI_B_CTLW0_MST |                  // Set as SPI master
                EUSCI_B_CTLW0_SYNC |                 // Set as synchronous mode
                EUSCI_B_CTLW0_CKPH |                 // Set clock polarity high
                EUSCI_B_CTLW0_MSB;                   // MSB first

    EUSCI_SPI_CTLW0 &= ~EUSCI_B_CTLW0_MODE_0;        // 3-pin SPI (default)

    EUSCI_SPI_CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK;    // SMCLK (12Mhz)

    EUSCI_SPI_BRW = 12;                              // 12 Mhz / 12 = 1 Mhz (confirm)

    EUSCI_SPI_CTLW0 &= ~EUSCI_B_CTLW0_SWRST;         // Initialize USCI state machine

    __delay_cycles(500000);

}


void init_lcd()
{
    LCD5110_RST_LOW;
    delay_usec(50000);
    LCD5110_RST_HIGH;

    write_to_lcd(LCD5110_COMMAND, PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION);
    write_to_lcd(LCD5110_COMMAND, PCD8544_SETVOP | 0x3F);
    write_to_lcd(LCD5110_COMMAND, PCD8544_SETTEMP | 0x02);
    write_to_lcd(LCD5110_COMMAND, PCD8544_SETBIAS | 0x03);
    write_to_lcd(LCD5110_COMMAND, PCD8544_FUNCTIONSET);
    write_to_lcd(LCD5110_COMMAND, PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);

    clear_lcd();

    set_address(0, 0);
    write_string_to_lcd("Sensor Monitor    System");
}


void write_to_lcd(uint8_t command, uint8_t data)
{
    /* select cs to be low */
    LCD5110_CHIP_SELECT;
    delay_usec(2);

    if(command)
    {
        LCD5110_SET_DATA;
    }
    else
    {
        LCD5110_SET_COMMAND;
    }

    /* write data into the transmit buffer */
    EUSCI_B2->TXBUF = data;

    /* check sequence confirm*/
    while(!(EUSCI_B2->IFG & EUSCI_B_IFG_TXIFG));

    /* added extra */
    while(EUSCI_B2->STATW & EUSCI_B_STATW_BBUSY) ;
    delay_usec(3);
    LCD5110_CHIP_DESELECT;
}

void write_char_to_lcd(uint8_t ch)
{
    uint8_t i = 0;

    for(i = 0; i < 5; i++)
    {
        write_to_lcd(LCD5110_DATA, font[ch - 0x20][i]);
    }

    write_to_lcd(LCD5110_DATA, 0);
}


void write_string_to_lcd(const char *string)
{
    while(*string)
    {
        write_char_to_lcd(*string++);
    }
}


void clear_lcd()
{
   set_address(0, 0);

   int c = 0;

   while(c < PCD8544_MAXBYTES)
   {
       write_to_lcd(LCD5110_DATA, 0);
       c++;
   }

   set_address(0, 0);
}


void set_address(uint8_t xaddress, uint8_t yaddress)
{
    write_to_lcd(LCD5110_COMMAND, PCD8544_SETXADDR | xaddress);
    write_to_lcd(LCD5110_COMMAND, PCD8544_SETYADDR | yaddress);
}

/* EOF */
