#include "config.h"

#if defined(ST7796U)

#include "spi.h"

#include <memory.h>
#include <stdio.h>

void InitST7796U()
{
  // If a Reset pin is defined, toggle it briefly high->low->high to enable the device. Some devices do not have a reset pin, in which case compile with GPIO_TFT_RESET_PIN left undefined.
#if defined(GPIO_TFT_RESET_PIN) && GPIO_TFT_RESET_PIN >= 0
  printf("Resetting display at reset GPIO pin %d\n", GPIO_TFT_RESET_PIN);
  SET_GPIO_MODE(GPIO_TFT_RESET_PIN, 1);
  SET_GPIO(GPIO_TFT_RESET_PIN);
  usleep(120 * 1000);
  CLEAR_GPIO(GPIO_TFT_RESET_PIN);
  usleep(120 * 1000);
  SET_GPIO(GPIO_TFT_RESET_PIN);
  usleep(120 * 1000);
#endif

  // Do the initialization with a very low SPI bus speed, so that it will succeed even if the bus speed chosen by the user is too high.
  spi->clk = 34;
  __sync_synchronize();

  BEGIN_SPI_COMMUNICATION();
  {

    usleep(120 * 1000);
    SPI_TRANSFER(0x11 /*Sleep Out*/);
    usleep(120 * 1000);

    SPI_TRANSFER(0x3A /*COLMOD: Pixel Format Set*/, 0x05 /*16bpp*/);
    usleep(20 * 1000);

#define MADCTL_BGR_PIXEL_ORDER (1 << 3)//8
#define MADCTL_ROW_COLUMN_EXCHANGE (1 << 5)//32
#define MADCTL_COLUMN_ADDRESS_ORDER_SWAP (1 << 6)//64
#define MADCTL_ROW_ADDRESS_ORDER_SWAP (1 << 7)//128
#define MADCTL_ROTATE_180_DEGREES (MADCTL_COLUMN_ADDRESS_ORDER_SWAP | MADCTL_ROW_ADDRESS_ORDER_SWAP)//192
//#define DISPLAY_SWAP_BGR 1
    uint8_t madctl = 0;
#ifdef DISPLAY_SWAP_BGR
    madctl ^= MADCTL_BGR_PIXEL_ORDER;
#endif
#if defined(DISPLAY_FLIP_ORIENTATION_IN_HARDWARE)
    madctl |= MADCTL_ROW_COLUMN_EXCHANGE;
#endif

//    madctl |= MADCTL_ROW_ADDRESS_ORDER_SWAP;


#ifdef DISPLAY_ROTATE_180_DEGREES
    madctl ^= MADCTL_ROTATE_180_DEGREES;
#endif


    SPI_TRANSFER(0x36 /*MADCTL: Memory Access Control*/, madctl);
    usleep(10 * 1000);
printf("3.5lcd  madctl:%02x\r\n",madctl);
#ifdef ST7796U
    //SPI_TRANSFER(0xBA/*DGMEN: Enable Gamma*/, 0x04);
    bool invertColors = true;
#else
    bool invertColors = false;
#endif
#ifdef DISPLAY_INVERT_COLORS
    invertColors = !invertColors;
#endif

    if (invertColors)
      SPI_TRANSFER(0x21 /*Display Inversion On*/);
    else
      SPI_TRANSFER(0x20 /*Display Inversion Off*/);

    SPI_TRANSFER(0x13 /*NORON: Partial off (normal)*/);
    usleep(10 * 1000);


#if defined(RZWSHARE_3INCH5_LCD)
printf("************rzwshare_3inch5_lcd******\r\n");


SPI_TRANSFER( 0xF0, 0xC3);   

SPI_TRANSFER( 0xF0 , 0x96);   

//SPI_TRANSFER(0x36, 0x48);   

SPI_TRANSFER( 0x3A, 0x55);   

SPI_TRANSFER( 0xB4,0x01);   

SPI_TRANSFER( 0xB7 , 0xC6);

SPI_TRANSFER( 0xC0, 0x80, 0x64); //VGH=15V VGL=-10V  

SPI_TRANSFER(0xC1, 0x13);  //VOP=4.5V

SPI_TRANSFER( 0xC2, 0xA7);   

SPI_TRANSFER( 0xC5, 0x08);   

SPI_TRANSFER( 0xE8, 0x40, 0x8a, 0x00, 0x00, 0x29, 0x19 , 0xA5 , 0x33);   
SPI_TRANSFER( 0xE0 , 0xF0 , 0x06, 0x0B, 0x07, 0x06 , 0x05, 0x2E , 0x33, 0x47, 0x3A , 0x17 , 0x16, 0x2E, 0x31);

SPI_TRANSFER( 0xE1, 0xF0, 0x09, 0x0D , 0x09, 0x08 , 0x23, 0x2E , 0x33, 0x46 , 0x38, 0x13, 0x13 , 0x2C , 0x32);

SPI_TRANSFER( 0xF0 , 0x3C);   

SPI_TRANSFER( 0xF0, 0x69);   

SPI_TRANSFER( 0x35, 0x00); 

SPI_TRANSFER( 0x21); 

SPI_TRANSFER( 0x29);     
usleep(50); 

SPI_TRANSFER( 0x2A, 0x00 , 0x00 , 0x01, 0x3F);   

SPI_TRANSFER( 0x2B, 0x00 , 0x00 , 0x01, 0xDF); 

SPI_TRANSFER( 0x2C); 

#endif

    SPI_TRANSFER(/*Display ON*/ 0x29);
    usleep(100 * 1000);


#if defined(GPIO_TFT_BACKLIGHT) && defined(BACKLIGHT_CONTROL)
    printf("Setting TFT backlight on at pin %d\n", GPIO_TFT_BACKLIGHT);
    SET_GPIO_MODE(GPIO_TFT_BACKLIGHT, 0x01); // Set backlight pin to digital 0/1 output mode (0x01) in case it had been PWM controlled
    SET_GPIO(GPIO_TFT_BACKLIGHT);            // And turn the backlight on.
#endif

    ClearScreen();
  }
#ifndef USE_DMA_TRANSFERS // For DMA transfers, keep SPI CS & TA active.
  END_SPI_COMMUNICATION();
#endif

  // And speed up to the desired operation speed finally after init is done.
  usleep(10 * 1000); // Delay a bit before restoring CLK, or otherwise this has been observed to cause the display not init if done back to back after the clear operation above.
  spi->clk = SPI_BUS_CLOCK_DIVISOR;
}

void TurnDisplayOff()
{
#if defined(GPIO_TFT_BACKLIGHT) && defined(BACKLIGHT_CONTROL)
  SET_GPIO_MODE(GPIO_TFT_BACKLIGHT, 0x01); // Set backlight pin to digital 0/1 output mode (0x01) in case it had been PWM controlled
  CLEAR_GPIO(GPIO_TFT_BACKLIGHT);          // And turn the backlight off.
#endif
#if 0
  QUEUE_SPI_TRANSFER(0x28/*Display OFF*/);
  QUEUE_SPI_TRANSFER(0x10/*Enter Sleep Mode*/);

  usleep(120*1000); // Sleep off can be sent 120msecs after entering sleep mode the earliest, so synchronously sleep here for that duration to be safe.
#endif
  //  printf("Turned display OFF\n");
}

void TurnDisplayOn()
{
#if 0
  QUEUE_SPI_TRANSFER(0x11/*Sleep Out*/);
  usleep(120 * 1000);
  QUEUE_SPI_TRANSFER(0x29/*Display ON*/);
#endif
#if defined(GPIO_TFT_BACKLIGHT) && defined(BACKLIGHT_CONTROL)
  SET_GPIO_MODE(GPIO_TFT_BACKLIGHT, 0x01); // Set backlight pin to digital 0/1 output mode (0x01) in case it had been PWM controlled
  SET_GPIO(GPIO_TFT_BACKLIGHT);            // And turn the backlight on.
#endif
  //  printf("Turned display ON\n");
}

void DeinitSPIDisplay()
{
  ClearScreen();
}

#endif
