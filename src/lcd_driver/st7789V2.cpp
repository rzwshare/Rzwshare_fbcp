#include "config.h"

#if defined(ST7789V2)

#include "spi.h"

#include <memory.h>
#include <stdio.h>

void InitST7789V2()
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

    uint8_t madctl = 0;
#ifdef DISPLAY_SWAP_BGR
    madctl ^= MADCTL_BGR_PIXEL_ORDER;
#endif
#if defined(DISPLAY_FLIP_ORIENTATION_IN_HARDWARE)
    madctl |= MADCTL_ROW_COLUMN_EXCHANGE;
#endif

    madctl |= MADCTL_ROW_ADDRESS_ORDER_SWAP;


#ifdef DISPLAY_ROTATE_180_DEGREES
    madctl ^= MADCTL_ROTATE_180_DEGREES;
#endif

#if defined(RZWSHARE_1INCH4_LCD)
    madctl ^= MADCTL_ROTATE_180_DEGREES;
#endif

    SPI_TRANSFER(0x36 /*MADCTL: Memory Access Control*/, madctl);
    usleep(10 * 1000);

#ifdef ST7789V2
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

#ifdef ST7789V2
    // The ST7789 controller is actually a unit with 320x240 graphics memory area, but only 240x240 portion
    // of it is displayed. Therefore if we wanted to swap row address mode above, writes to Y=0...239 range will actually land in
    // memory in row addresses Y = 319-(0...239) = 319...80 range. To view this range, we must scroll the view by +80 units in Y
    // direction so that contents of Y=80...319 is displayed instead of Y=0...239.
    
    if ((madctl & MADCTL_ROW_ADDRESS_ORDER_SWAP))
      SPI_TRANSFER(0x37 /*VSCSAD: Vertical Scroll Start Address of RAM*/, 0, 320 - DISPLAY_WIDTH);
    //printf("DISPLAY_HEIGHT =%d\r\n",DISPLAY_HEIGHT);
#endif


#if defined(RZWSHARE_1INCH4_LCD)
printf("************rzwshare_1inch4_lcd******\r\n");
 	SPI_TRANSFER(0x2A, 0x00, 0x00, 0x00,0xEF);
 	SPI_TRANSFER(0x2B, 0x00, 0x00, 0x00,0xEF);
	
    SPI_TRANSFER(0xB2, 0x0c, 0x0c, 0x00, 0x33, 0x33);
    SPI_TRANSFER(0xB7, 0x00);
    SPI_TRANSFER(0xBb, 0x36);
    SPI_TRANSFER(0xc0, 0x2c);
    SPI_TRANSFER(0xc2, 0x01);
    SPI_TRANSFER(0xc3, 0x13);
    SPI_TRANSFER(0xc4, 0x20);
    SPI_TRANSFER(0xc6, 0x0f);
	SPI_TRANSFER(0xd6, 0xa1);
    SPI_TRANSFER(0xd0, 0xa4, 0xa1);
    SPI_TRANSFER(0xe0, 0xF0, 0x08, 0x0E, 0x09, 0x08, 0x4, 0x2f, 0x33, 0x45, 0x36, 0x13, 0x12, 0x2A, 0x2D);
 
	SPI_TRANSFER(0xe1, 0xF0, 0x0E, 0x12, 0x0C, 0x0A, 0x15, 0x2E, 0x32, 0x44, 0x39, 0x17, 0x18, 0x2B,0x2F);
    SPI_TRANSFER(0x21);
   
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
