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
#include "bsp_systick.h"
#include "letter-shell/src/shell.h"
#include "coremark/coremark.h"
#include "perf_counter/perf_counter.h"
#include "elog.h"
/* TODO: insert other include files here. */

extern Shell shell;

static BUTTON_HANDLE_DEFINE(s_buttonHandle);

button_status_t button_cb(void *buttonHandle,
                                             button_callback_message_t *message,
                                             void *callbackParam)
{
	static int tri = 0;
	if (tri == 0)
	{
		RGB_LED_COLOR_GREEN
		tri = 1;
	} else
	{
		RGB_LED_COLOR_BLUE
		tri = 0;
	}
	PRINTF("message event:%d\n", message->event);

	return kStatus_BUTTON_Success;
}

/*
 * @brief   Application entry point.
 */
void userShellInit(void);
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

    SystemCoreClockUpdate();
    PRINTF("*****>>welcome i.MX RT1052 develop board <<*****\r\n");
    PRINTF("CPU:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_CpuClk));
    PRINTF("AHB:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_AhbClk));
    PRINTF("SEMC:            %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SemcClk));
    PRINTF("SYSPLL:          %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllClk));
    PRINTF("SYSPLLPFD0:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd0Clk));
    PRINTF("SYSPLLPFD1:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd1Clk));
    PRINTF("SYSPLLPFD2:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk));
    PRINTF("SYSPLLPFD3:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd3Clk));
    PRINTF("RT1025 SystemCoreClock=%dMhz\n", SystemCoreClock/1000000);

    /* initialize EasyLogger */
    elog_init();
    /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    /* start EasyLogger */
    elog_start();

//    SysTick_Init();
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
	 userShellInit();
//	 SCB_CleanDCache();
//	 SCB_DisableDCache();
	 SCB_EnableICache();
	 SCB_EnableDCache();

	    /*! \brief Update SystemCoreClock with the latest CPU frequency
	     *!        If the function doesn't exist or doesn't work correctly,
	     *!        Please update SystemCoreClock directly with the correct
	     *!        system frequency in Hz.
	     *!
	     *!        extern volatile uint32_t SystemCoreClock;
	     */
//	    SystemCoreClockUpdate();

	    /*! \brief initialize perf_counter() and pass true if SysTick is
	     *!        occupied by user applications or RTOS, otherwise pass
	     *!        false.
	     */
	init_cycle_counter(false);
//	coremark_main();

	start_cycle_counter(); {
	    PRINTF("Hello world\r\n");
	}
	int32_t cycles = stop_cycle_counter();
	PRINTF("cycles:%d\n", cycles);
    while(1)
    {
//    	shellTask(&shell);
    	RGB_LED_COLOR_YELLOW
		CORE_BOARD_LED(1);
    	delay_ms(500);
    	RGB_LED_COLOR_BLUE
		CORE_BOARD_LED(0);
//    	delay_ms(500);
    	delay_us(500*1000);
        if (perfc_is_time_out_ms(1000)) {
            /* print hello world every 1000 ms */
        	log_i("systick:%lld", get_system_ms());
        }

        __cpu_usage__(5, {
            float fUsage = __usage__; /*< "__usage__" stores the result */
            log_i("task 1 cpu usage %f", fUsage);
        }) {
        	delay_ms(1000);
        }

        /* measure cycles and store it in a dedicated variable without printf */
//        int32_t iCycleResult = 0;
//        __cycleof__("delay_us(1000ul)",
//            /* insert code to __cycleof__ body, "{}" can be omitted  */
//            {
//                iCycleResult = __cycle_count__;   /*< "__cycle_count__" stores the result */
//            }) {
//            delay_us(1000ul);
//        }
//
//        PRINTF("\r\n delay_us(1000ul) takes %d cycles\r\n", (int)iCycleResult);
    }
    return 0 ;
}
