/*
 * bsp_pit.h
 *
 *  Created on: 2025年1月11日
 *      Author: yongjie
 */

#ifndef BSP_PIT_H_
#define BSP_PIT_H_

#include "fsl_pit.h"

// 定时器时钟频率宏定义  perclk 75M,不分频
#define PIT_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_PerClk)

//中断宏定义
#define PIT_IRQ_ID PIT_IRQn
#define PIT_LED_HANDLER PIT_IRQHandler

//定时器通道宏定义
#define PIT_CHANNEL_X kPIT_Chnl_0

//计时时间宏定义 us
#define TIME_0 1000000U

void PIT_TIMER_Init(void);



#endif /* BSP_PIT_H_ */
