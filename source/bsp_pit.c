/*
 * bsp_pit.c
 *
 *  Created on: 2025年1月11日
 *      Author: yongjie
 */


#include "bsp_led.h"
#include "bsp_pit.h"
#include "bsp_nvic.h"
#include "fsl_adapter_timer.h"
#include "fsl_component_timer_manager.h"
#include "elog.h"
#include "perf_counter/perf_counter.h"

volatile uint64_t k = 0;

//TIMER_HANDLE_DEFINE(led_timer);
TIMER_MANAGER_HANDLE_DEFINE(timerHandle);

int period_1ms_flag = 0;
int period_2ms_flag = 0;
int period_5ms_flag = 0;
int period_10ms_flag = 0;
int period_50ms_flag = 0;
int period_100ms_flag = 0;
int period_500ms_flag = 0;
int period_1000ms_flag = 0;

void led_timer_cb(void* param)
{
	k++;
	if (k % 1 == 0)
	{
		period_1ms_flag = 1;
	}
	if (k % 2 == 0)
	{
		period_2ms_flag = 1;
	}
	if (k % 5 == 0)
	{
		period_5ms_flag = 1;
	}
	if (k % 10 == 0)
	{
		period_10ms_flag = 1;
	}
	if (k % 50 == 0)
	{
		period_50ms_flag = 1;
	}
	if (k % 100 == 0)
	{
		period_100ms_flag = 1;
	}
	if (k % 500 == 0)
	{
		period_500ms_flag = 1;
	}
	if (k % 1000 == 0)
	{
		period_1000ms_flag = 1;
	}
}

void PIT_TIMER_Init(void)
{
//	  CLOCK_SetMux(kCLOCK_PerclkMux, 0U);
//	  CLOCK_SetDiv(kCLOCK_PerclkDiv, 0U);
	// 初始化75M 0/1
	  log_i("kCLOCK_PerClk:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_PerClk));

	// use pit_adapter
//	hal_timer_handle_t timer_handle = &led_timer[0];
//	hal_timer_config_t halTimerConfig;

//	halTimerConfig.clockSrcSelect = ;
//	halTimerConfig.instance = 0;
//	halTimerConfig.srcClock_Hz = CLOCK_GetFreq(kCLOCK_OscClk);
//	halTimerConfig.timeout = 1000000U;
//
//	hal_timer_status_t status = HAL_TimerInit(timer_handle, &halTimerConfig);
//	assert(kStatus_HAL_TimerSuccess == status);
//
//	HAL_TimerInstallCallback(timer_handle, led_timer_cb, NULL);
//	HAL_TimerEnable(timer_handle);

	 // usb timer_manager
	timer_config_t timer_config;
	timer_config.instance = 0;
	timer_config.srcClock_Hz = CLOCK_GetFreq(kCLOCK_PerClk);

	timer_status_t  status = TM_Init(&timer_config);
	assert(kStatus_TimerSuccess == status);

	status = TM_Open(timerHandle);
	assert(kStatus_TimerSuccess == status);

	status = TM_InstallCallback(timerHandle, led_timer_cb, NULL);
    assert(kStatus_TimerSuccess == status);

    status = TM_Start(timerHandle, (uint8_t)kTimerModeIntervalTimer, 1);
    assert(kStatus_TimerSuccess == status);

//  pit_config_t pitConfig;
//
//  CLOCK_SetMux(kCLOCK_PerclkMux, 1U);
//  CLOCK_SetDiv(kCLOCK_PerclkDiv, 0U);
//
//  PIT_GetDefaultConfig(&pitConfig);
//  PIT_Init(PIT, &pitConfig);
//
//  PIT_SetTimerPeriod(PIT, PIT_CHANNEL_X, USEC_TO_COUNT(TIME_0, PIT_SOURCE_CLOCK));
//
//  PIT_ClearStatusFlags(PIT, PIT_CHANNEL_X, kPIT_TimerFlag);
//  PIT_EnableInterrupts(PIT, PIT_CHANNEL_X, kPIT_TimerInterruptEnable);
//
//  set_IRQn_Priority(PIT_IRQ_ID,Group4_PreemptPriority_6, Group4_SubPriority_0);
//  EnableIRQ(PIT_IRQ_ID);
}
