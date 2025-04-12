/*
 * bsp_pit.c
 *
 *  Created on: 2025年1月11日
 *      Author: yongjie
 */


#include <bsp/os_schedule.h>
#include "fsl_adapter_timer.h"
#include "fsl_component_timer_manager.h"
#include "elog.h"
#include "perf_counter/perf_counter.h"
#include "bsp_led.h"

volatile uint64_t k = 0;

TIMER_MANAGER_HANDLE_DEFINE(timerHandle2);

int period_1ms_flag = 0;
int period_2ms_flag = 0;
int period_5ms_flag = 0;
int period_10ms_flag = 0;
int period_20ms_flag = 0;
int period_50ms_flag = 0;
int period_100ms_flag = 0;
int period_200ms_flag = 0;
int period_500ms_flag = 0;
int period_1000ms_flag = 0;

void schedule_timer_cb(void* param)
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

void OS_TIMER_Init(void)
{
	timer_config_t timer_config;
	timer_config.instance = 1;
	timer_config.srcClock_Hz = CLOCK_GetFreq(kCLOCK_PerClk);

	timer_status_t  status = TM_Init(&timer_config);
	ELOG_ASSERT(kStatus_TimerSuccess == status);

	status = TM_Open(timerHandle2);
	ELOG_ASSERT(kStatus_TimerSuccess == status);

	status = TM_InstallCallback(timerHandle2, schedule_timer_cb, NULL);
	ELOG_ASSERT(kStatus_TimerSuccess == status);

    status = TM_Start(timerHandle2, (uint8_t)kTimerModeIntervalTimer, 1);
    ELOG_ASSERT(kStatus_TimerSuccess == status);
}

void sys_1ms_task(void)
{

}

void sys_2ms_task(void)
{

}

void sys_5ms_task(void)
{

}

void sys_10ms_task(void)
{

}

void sys_20ms_task(void)
{

}

void sys_50ms_task(void)
{

}

void sys_100ms_task(void)
{

}

void sys_200ms_task(void)
{
}

void sys_500ms_task(void)
{
//	RGB_LED_COLOR_RED;
}

void sys_1000ms_task(void)
{
	log_i("%s:%d enter!", __func__, __LINE__);
}

void OS_Schedule(void)
{
	if (period_1ms_flag) { period_1ms_flag = 0; sys_1ms_task();}
	if (period_2ms_flag) { period_2ms_flag = 0; sys_2ms_task();}
	if (period_5ms_flag) { period_5ms_flag = 0; sys_5ms_task();}
	if (period_10ms_flag) { period_10ms_flag = 0; sys_10ms_task();}
	if (period_20ms_flag) { period_20ms_flag = 0; sys_20ms_task();}
	if (period_50ms_flag) { period_50ms_flag = 0; sys_50ms_task();}
	if (period_100ms_flag) { period_100ms_flag = 0; sys_100ms_task();}
	if (period_200ms_flag) { period_200ms_flag = 0; sys_200ms_task();}
	if (period_500ms_flag) { period_500ms_flag = 0; sys_500ms_task();}
	if (period_1000ms_flag) { period_1000ms_flag = 0; sys_1000ms_task();}
}
