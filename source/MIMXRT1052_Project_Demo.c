/*
 * Copyright 2016-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    MIMXRT1052_Project_Demo.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MIMXRT1052.h"
#include "fsl_debug_console.h"
#include "cm_backtrace.h"
#include "systick_utils.h"
#include "perf_counter.h"
/* TODO: insert other include files here. */

/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */
int main(void) {

    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif
    cm_backtrace_init("CmBacktrace for i.MX RT1052 EVK Pro Cortex-M7", "1.0", "1.0");
    SystemCoreClockUpdate();
    BOARD_SystickEnable();
    init_cycle_counter(true);
    PRINTF("Hello World\r\n");

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
        /* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
        __asm volatile ("nop");
        delay_ms(1000);
//        PRINTF("systick:%d\r\n", BOARD_GetTick());
        PRINTF("perf tick:%lld, ms:%lld\r\n", get_system_ticks(), get_system_ms());
    }
    return 0 ;
}
