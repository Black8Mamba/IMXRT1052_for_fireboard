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
#include "cm_backtrace.h"
#include "systick_utils.h"
#include "perf_counter.h"
#include "letter-shell/src/shell.h"
#include "elog.h"
#include "coremark/coremark.h"
#include "os_schedule.h"
#include "boot.h"
#include "types.h"
#include "tapdev.h"
#include "uip.h"
/* TODO: insert other include files here. */
extern void userShellInit(void);
/* TODO: insert other definitions and declarations here. */
extern Shell shell;

extern uint32_t __VECTOR_TABLE[];

void relocate_vector_table(void)
{
    __disable_irq();
    memcpy((void *)0x2001F000, (void *)__VECTOR_TABLE, 0x400);
    SCB->VTOR = 0x2001F000;
    __enable_irq();
}
extern Shell shell;
void Rs232TransmitByte(blt_int8u data);
void Rs232TransmitPacket(blt_int8u *data, blt_int8u len);
blt_bool Rs232ReceiveByte(blt_int8u *data);

void relocate_vector_table_app(void)
{
    __disable_irq();
    memcpy((void *)0x2001F000, (void *)0x60042000, 0x400);
    SCB->VTOR = 0x2001F000;
    __enable_irq();
}

void pre_jump_app(void)
{
	// disable global interrupt
	__disable_irq();

	enet_deinit();
	DbgConsole_Deinit();

    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

	//clear pending irq
	for(int i = -14; i <= 151; ++i)
	{
		NVIC_ClearPendingIRQ((IRQn_Type)i);
	}

	relocate_vector_table_app();
	SCB_CleanDCache();
	SCB_DisableICache();
	SCB_DisableDCache();
	ARM_MPU_Disable();
}

#define APP_ADDRESS 0x60042000 // APP起始地址(根据实际修改)

typedef void (*pFunction)(void);

void jump_to_app_3(void)
{
    uint32_t jump_address;
    pFunction jump_to_application;

    // 检查APP地址是否有效(栈顶地址在RAM范围内)
//    if(((*(__IO uint32_t*)APP_ADDRESS) & 0x2FFE0000) == 0x20000000)
    if (1)
    {
        // 禁用所有中断
        __disable_irq();

        // 重置SysTick定时器
        SysTick->CTRL = 0;
        SysTick->LOAD = 0;
        SysTick->VAL = 0;

        // 设置主堆栈指针
        __set_MSP(*(__IO uint32_t*)APP_ADDRESS);

        // 获取复位处理函数地址
        jump_address = *(__IO uint32_t*)(APP_ADDRESS + 4);
        jump_to_application = (pFunction)jump_address;

        // 跳转到APP
        jump_to_application();
    }
    // 如果无效则停留在bootloader
}

/*
 * @brief   Application entry point.
 */
struct uip_udp_conn *udp_conn_send;
struct uip_udp_conn *udp_conn_recv;
int main(void) {
	relocate_vector_table();

    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();

    int FlexSPI_NorFlash_Init(void);
    FlexSPI_NorFlash_Init();


    //	jump_to_app();
    //	uint8_t  *p = 0x600c0000;
    //
    //	uint8_t flag = *p;
    //
    //	if (*p == 0x5a)
    //	{
    //		uint8_t buffer[256] = {0};
    //		memcpy(0x600c0000, buffer, 256);
    //		jump_to_app();
    //	}


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
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_FUNC);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~ELOG_FMT_DIR & ~ELOG_FMT_P_INFO & ~ELOG_FMT_T_INFO);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~ELOG_FMT_FUNC);
    elog_start();
    log_i("Hello World bootloader jiyongjie");
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
//    coremark_main();
//    OS_TIMER_Init();
    void led_test(void);
    	led_test();
//    int FlexSPI_NorFlash_Init(void);
//    FlexSPI_NorFlash_Init();
    BootInit();
//    uip_init();
//    uip_ipaddr_t ipaddr;
//    uip_ipaddr(ipaddr, 192, 168, 1, 172);//MCU IP地址
//    uip_sethostaddr(ipaddr);
//    uip_ipaddr(ipaddr, 192, 168, 1, 2);//网关,电脑和MCU经过路由器时使用
//    uip_setdraddr(ipaddr);
//    uip_ipaddr(ipaddr, 255, 255, 252, 0);//子网掩码
//    uip_setnetmask(ipaddr);
//
//    uip_ipaddr_t udp_ipaddr;
//    uip_ipaddr(udp_ipaddr, 0,0,0,0);
//    udp_conn_recv = uip_udp_new(&udp_ipaddr, 0);
//    if (udp_conn_recv != NULL) {
//        uip_udp_bind(udp_conn_recv, HTONS(8888));
//    }
//
//    uip_ipaddr_t udp_ipaddr_send;
//    uip_ipaddr(udp_ipaddr_send, 192,168,1,171);
//    udp_conn_send = uip_udp_new(&udp_ipaddr_send, HTONS(8888));
//    if (udp_conn_send != NULL) {
//        uip_udp_bind(udp_conn_send, HTONS(8888));
//    }
    while(1) {
    	OS_Schedule();
    	BootTask();
    }
    return 0 ;
}
