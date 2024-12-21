/*
 * Copyright 2016-2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    MIMXRT1052_Project.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MIMXRT1052.h"
#include "fsl_debug_console.h"

#include "bsp_led.h"
#include "bsp_key.h"
#include "fsl_shell.h"

#include "fsl_component_button.h"
/* TODO: insert other include files here. */
char c = 'N';

static BUTTON_HANDLE_DEFINE(s_buttonHandle);

button_status_t button_cb(void *buttonHandle,
                                             button_callback_message_t *message,
                                             void *callbackParam)
{
	static tri = 0;
	if (tri == 0)
	{
		RGB_LED_COLOR_GREEN
		tri = 1;
	} else
	{
		RGB_LED_COLOR_BLUE
		tri = 0;
	}
	PRINTF("message event:%d, last char:%c\r\n", message->event, c);
}

/*
 * @brief   Application entry point.
 */
static SHELL_HANDLE_DEFINE(s_shellHandle);
int main(void) {

    /* Init board hardware. */
//    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    PRINTF("*****>>welcome i.MX RT1052 develop board <<*****\r\n");
    PRINTF("CPU:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_CpuClk));
    PRINTF("AHB:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_AhbClk));
    PRINTF("SEMC:            %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SemcClk));
    PRINTF("SYSPLL:          %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllClk));
    PRINTF("SYSPLLPFD0:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd0Clk));
    PRINTF("SYSPLLPFD1:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd1Clk));
    PRINTF("SYSPLLPFD2:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk));
    PRINTF("SYSPLLPFD3:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd3Clk));

//    RGB_LED_COLOR_PURPLE
    RGB_LED_COLOR_YELLOW

	 button_config_t buttonConfig;
	 buttonConfig.gpio.port = 1;
	 buttonConfig.gpio.pin = 5;
	 buttonConfig.gpio.pinStateDefault = 1;
	 buttonConfig.gpio.direction = kHAL_GpioDirectionIn;
	 if (BUTTON_Init((button_handle_t)s_buttonHandle, &buttonConfig) != kStatus_BUTTON_Success)
	 {
		 PRINTF("BUTTON_Init failed!\r\n");
	 } else
	 {
		 PRINTF("BUTTON_Init success!\r\n");
	 }

	 if (BUTTON_InstallCallback((button_handle_t)s_buttonHandle, button_cb, NULL) != kStatus_BUTTON_Success)
	 {
		 PRINTF("BUTTON_InstallCallback failed!\r\n");
	 }


    while(1)
    {
    	c = DbgConsole_Getchar();
    	PRINTF("char:%c\r\n", c);
    }




//    char *p = 0x80000000;
//    *p = 't';
//    PRINTF("Hello World:%c\r\n", *p);
    PRINTF("Hello World IMX1052!\r\n");

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
        /* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
        __asm volatile ("nop");
    }
    return 0 ;
}
