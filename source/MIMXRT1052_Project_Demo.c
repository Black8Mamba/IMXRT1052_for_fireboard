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
#define LOG_TAG    "main"
#define LOG_LVL    ELOG_LVL_DEBUG

#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MIMXRT1052.h"
#include "fsl_debug_console.h"
#include "fsl_component_log.h"
#include "cm_backtrace.h"
#include "systick_utils.h"
#include "perf_counter.h"
#include "letter-shell/src/shell.h"
#include "elog.h"
#include "coremark/coremark.h"
#include "os_schedule.h"
/* TODO: insert other include files here. */
extern void userShellInit(void);
/* TODO: insert other definitions and declarations here. */
extern Shell shell;
/*
 * @brief   Application entry point.
 */

static void uart_backend(uint8_t *buffer, size_t length)
{
	DbgConsole_SendDataReliable((uint8_t *)buffer, length);
}


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
    userShellInit();
    elog_init();
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_FUNC | ELOG_FMT_FUNC);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_FUNC | ELOG_FMT_FUNC);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_DIR & ~ELOG_FMT_P_INFO & ~ELOG_FMT_T_INFO);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    elog_start();
    log_i("Hello World");
    elog_raw("*****>>welcome i.MX RT1052 develop board<<*****\r\n");
    elog_raw("CPU:             %ud Hz\r\n", CLOCK_GetFreq(kCLOCK_CpuClk));
    elog_raw("AHB:             %ud Hz\r\n", CLOCK_GetFreq(kCLOCK_AhbClk));
    elog_raw("SEMC:            %ud Hz\r\n", CLOCK_GetFreq(kCLOCK_SemcClk));
    elog_raw("SYSPLL:          %ud Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllClk));
    elog_raw("perClk:          %ud Hz\r\n", CLOCK_GetFreq(kCLOCK_PerClk));
    elog_raw("SYSPLLPFD0:      %ud Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd0Clk));
    elog_raw("SYSPLLPFD1:      %ud Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd1Clk));
    elog_raw("SYSPLLPFD2:      %ud Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk));
    elog_raw("SYSPLLPFD3:      %ud Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd3Clk));
    elog_raw("SYSPLLPFD3:      %ud Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd3Clk));
    elog_raw("RT1025 SystemCoreClock=%dMhz\r\n", SystemCoreClock/1000000);
	LOG_Init();
	LOG_MODULE_DEFINE(main, kLOG_LevelDebug);
	LOG_BACKEND_DEFINE(log, uart_backend);
	LOG_BackendRegister(&log);
//    coremark_main();
    OS_TIMER_Init();
    void led_test(void);
    led_test();
    while(1) {
    	OS_Schedule();
    }
    return 0 ;
}
