#pragma once

// Data specific to RzwShare 320x480, 3.5inch IPS LCD ST7796U 
#ifdef RZWSHARE_3INCH5_LCD

    #ifndef __RAZWSHARE_3INCH5_LCD_H

        #define __RZWSHARE_3INCH5_LCD_H
	#define DISPLAY_SWAP_BGR 
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
            #define DISPLAY_NATIVE_WIDTH 320//240
        #endif

        #if !defined (SET_DISPLAY_HEIGHT)
            #define DISPLAY_NATIVE_HEIGHT 480//240//320
        #endif

    #endif

#endif
