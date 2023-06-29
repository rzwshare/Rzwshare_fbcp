#pragma once

// Data specific to RzwShare 240x320, 1.4inch IPS LCD ST7789V2
#ifdef RZWSHARE_1INCH4_LCD

    #ifndef __RZWSHARE_1INCH4_LCD_H

        #define __RZWSHARE_1INCH4_LCD_H

        #if !defined(GPIO_TFT_DATA_CONTROL)
            #define GPIO_TFT_DATA_CONTROL 25
        #endif

        #if !defined(GPIO_TFT_BACKLIGHT)
            #define GPIO_TFT_BACKLIGHT 24
        #endif

        #if !defined(GPIO_TFT_RESET_PIN)
            #define GPIO_TFT_RESET_PIN 27
        #endif
        
        #if !defined (SET_DISPLAY_WIDTH)
            #define DISPLAY_NATIVE_WIDTH 240
        #endif

        #if !defined (SET_DISPLAY_HEIGHT)
            #define DISPLAY_NATIVE_HEIGHT 240//320
        #endif

    #endif

#endif
