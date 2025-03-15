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
#include "cm_backtrace.h"
#include "bsp_i2c_eeprom.h"
#include "bsp_nvic.h"
#include "bsp_pit.h"
#include "bsp_gpt.h"
#include "bsp_qtmr.h"
#include "bsp_adc.h"
#include "bsp_snvs_hp_rtc.h"
#include "bsp_can.h"
#include "bsp_sdram.h"
#include "bsp_nand.h"
#include "bsp_sd.h"
#include "bsp_sd_fatfs_test.h"
#include "bsp_norflash.h"

#include "easyflash.h"
/* TODO: insert other include files here. */

extern int period_1ms_flag;
extern int period_2ms_flag;
extern int period_5ms_flag;
extern int period_10ms_flag;
extern int period_50ms_flag;
extern int period_100ms_flag;
extern int period_500ms_flag;
extern int period_1000ms_flag;

extern Shell shell;

static BUTTON_HANDLE_DEFINE(s_buttonHandle);

extern volatile uint32_t qtmrIsrFlag;

adc_channel_config_t adcChannelConfigStruct;
float ADC_ConvertedValueLocal = 0;

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
extern void fault_test_by_unalign(void);
extern void fault_test_by_div0(void);
extern const int CMB_CSTACK_BLOCK_START;
extern const int CMB_CSTACK_BLOCK_END;
extern const int CMB_CODE_SECTION_START;
extern const int CMB_CODE_SECTION_END;
extern volatile bool ADC_ConversionDoneFlag;
extern volatile uint32_t ADC_ConvertedValue;
void userShellInit(void);

extern bool SEMC_SDRAMReadWriteTest(void);
extern void SEMC_SDRAMReadWriteSpeedTest(void);
extern bool SDRAM_FullChipTest(void);
extern void nand_flash_test(void);
void nand_flash_init(void);

#define BUFFER_SIZE (100U)
#define SDMMCHOST_DMA_BUFFER_ADDR_ALIGN 4
SDK_ALIGN(uint8_t g_bufferWrite[SDK_SIZEALIGN(BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));
SDK_ALIGN(uint8_t g_bufferRead[SDK_SIZEALIGN(BUFFER_SIZE, SDMMC_DATA_BUFFER_ALIGN_CACHE)],
          MAX(SDMMC_DATA_BUFFER_ALIGN_CACHE, SDMMCHOST_DMA_BUFFER_ADDR_ALIGN));

void sys_1ms_task(void)
{

}

void sys_2ms_task(void)
{

}

void sys_5ms_task(void)
{
//	shellTask(&shell);
}

void sys_10ms_task(void)
{

}

void sys_50ms_task(void)
{

}

void sys_100ms_task(void)
{

}

#include <cr_section_macros.h>

//__RAMFUNC(BOARD_SDRAM)
void sys_500ms_task(void)
{
	static int flag = 1;

	flag = !flag;
	if (flag == 1)
	{
//		EfErrCode err = ef_set_and_save_env("testenv", "test2");
		RGB_LED_COLOR_YELLOW
	} else
	{
//		EfErrCode err = ef_set_and_save_env("testenv", "test1");
		RGB_LED_COLOR_GREEN;
	}
//	PRINTF("testenv is %s!\n", ef_get_env("testenv"));
//	PRINTF("password is %s!\n", ef_get_env("password"));
}

void sys_1000ms_task(void)
{
//	RTC_TimeAndDate_Show();
//	ADC_ConversionDoneFlag = true;
//	adcChannelConfigStruct.channelNumber = 0;
//	adcChannelConfigStruct.enableInterruptOnConversionCompleted = true;
//	if (ADC_ConversionDoneFlag == true)
//	{
//		ADC_ConversionDoneFlag = false;
//    	ADC_SetChannelConfig(ADC1_PERIPHERAL, ADC1_CH0_CONTROL_GROUP, &adcChannelConfigStruct);
//
//    	log_i("The Conversion Value:%d\n", ADC_ConvertedValue);
//    	ADC_ConvertedValueLocal =((float)ADC_ConvertedValue)/4095.0f*3.3f;
//    	log_i("The current AD value:%f V\n", ADC_ConvertedValueLocal);
//	}
//	loop_test();
}


extern uint32_t __VECTOR_TABLE[];

void relocate_vector_table(void)
{
    __disable_irq();
    memcpy((void *)0x2001F000, (void *)__VECTOR_TABLE, 0x400);
    SCB->VTOR = 0x2001F000;
    __enable_irq();
}

int main(void) {
	relocate_vector_table();
    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif
//	SDRAM_Init();
    BOARD_InitNand();

    cm_backtrace_init("CmBacktrace for i.MX RT1052 EVK Pro Cortex-M7", "1.0", "1.0");
    SystemCoreClockUpdate();
    PRINTF("*****>>welcome i.MX RT1052 develop board<<*****\n");
    PRINTF("CPU:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_CpuClk));
    PRINTF("AHB:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_AhbClk));
    PRINTF("SEMC:            %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SemcClk));
    PRINTF("SYSPLL:          %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllClk));
    PRINTF("SYSPLLPFD0:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd0Clk));
    PRINTF("SYSPLLPFD1:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd1Clk));
    PRINTF("SYSPLLPFD2:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk));
    PRINTF("SYSPLLPFD3:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd3Clk));
    PRINTF("SYSPLLPFD3:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd3Clk));
    PRINTF("SEMCCLK:         %d Hz\r\n", EXAMPLE_SEMC_CLK_FREQ);
    PRINTF("RT1025 SystemCoreClock=%dMhz\n", SystemCoreClock/1000000);
    init_cycle_counter(false);

    if (easyflash_init() != EF_NO_ERR)
    {
    	PRINTF("easyflash_init failed!\n");
    }

    PRINTF("username:%s\n", ef_get_env("username"));


    if (ef_get_env("testenv") == NULL)
    {
    	PRINTF("testenv is NULL!\n");
    	EfErrCode err = ef_set_and_save_env("testenv", "test1");
	    if (err != EF_NO_ERR)
	    {
	    	PRINTF("set test env failed!\n");
	    } else
	    {
	    	PRINTF("get test env:%s\n", ef_get_env("testenv"));
	    }
    } else
    {
    	PRINTF("testenv is %s!\n", ef_get_env("testenv"));
    }

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
//	 SCB_EnableICache();
//	 SCB_EnableDCache();

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
//	coremark_main();

	start_cycle_counter(); {
	    PRINTF("Hello world\r\n");
	}
	int32_t cycles = stop_cycle_counter();
	PRINTF("cycles:%d\n", cycles);

	uint32_t main_stack_start_addr = (uint32_t)(&CMB_CSTACK_BLOCK_START);
	uint32_t main_stack_size = (uint32_t)(&CMB_CSTACK_BLOCK_END) - main_stack_start_addr;
	uint32_t code_start_addr = (uint32_t)(&CMB_CODE_SECTION_START);
	uint32_t code_size = (uint32_t)(&CMB_CODE_SECTION_END) - code_start_addr;
	PRINTF("main_stack_start_addr:%x, main_stack_size:%x, code_start_addr:%x, code_size:%x\n", main_stack_start_addr, main_stack_size, code_start_addr, code_size);
	CORE_BOARD_LED(0);

	Set_NVIC_PriorityGroup(Group_4);
	PIT_TIMER_Init();
	GPT_Config();
	delay_ms(500);
	TMR_Init();
//	ADC1_init();
	SNVS_init();
//	EEPROM_Test();
//	CAN_Config();

//	if(SEMC_SDRAMReadWriteTest() && SDRAM_FullChipTest())
//	if (SEMC_SDRAMReadWriteTest()&& SDRAM_FullChipTest())
//	{
//		PRINTF("test sdram pass!\n");
//	} else
//	{
//		PRINTF("test sdram fail!\n");
//	}

//	nand_flash_test();
//	nand_flash_init();
//	SDCard_Init();
//	SDCard_Test();
//	FATFS g_fileSystem;
//	f_mount_test(&g_fileSystem);
//	f_mkdir_test("/dir_1");
//	f_touch_test("/dir_1/gin.txt");
//	FIL file_object;
//	DIR dir_object;
//	FILINFO file_info;
//	f_readdir_test("/dir_1",&dir_object,&file_info);
//	memset(g_bufferWrite, 'a', sizeof(g_bufferWrite));
//	g_bufferWrite[BUFFER_SIZE - 2U] = '\r';
//	g_bufferWrite[BUFFER_SIZE - 1U] = '\n';
//	f_write_read_test("/dir_1/gin.txt", g_bufferWrite, g_bufferRead);

	FlexSPI_NorFlash_Init();
	int NorFlash_IPCommand_Test(void);
	NorFlash_IPCommand_Test();
	int NorFlash_AHBCommand_Test(void);
	NorFlash_AHBCommand_Test();
	while(1)
	{
		if (period_1ms_flag)
		{
			period_1ms_flag = 0;
			sys_1ms_task();
		}

		if (period_2ms_flag)
		{
			period_2ms_flag = 0;
			sys_2ms_task();
		}

		if (period_5ms_flag)
		{
			period_5ms_flag = 0;
			sys_5ms_task();
		}

		if (period_10ms_flag)
		{
			period_10ms_flag = 0;
			sys_10ms_task();
		}

		if (period_50ms_flag)
		{
			period_50ms_flag = 0;
			sys_50ms_task();
		}

		if (period_100ms_flag)
		{
			period_100ms_flag = 0;
			sys_100ms_task();
		}

		if (period_500ms_flag)
		{
			period_500ms_flag = 0;
			sys_500ms_task();
		}

		if (period_1000ms_flag)
		{
			period_1000ms_flag = 0;
			sys_1000ms_task();
		}
	}
}

//    while(1)
//    {

//    	fault_test_by_div0();
//    	fault_test_by_unalign();
//    	delay_us(500*1000);
//        if (perfc_is_time_out_ms(1000)) {
//            /* print hello world every 1000 ms */
//        	log_i("systick:%lld", get_system_ms());
//        }
//
//        __cpu_usage__(5, {
//            float fUsage = __usage__; /*< "__usage__" stores the result */
//            log_i("task 1 cpu usage %f", fUsage);
//        }) {
//        	delay_ms(1000);
//        }

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
//    }
//    return 0 ;
