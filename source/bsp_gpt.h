/*
 * bsp_gpt.h
 *
 *  Created on: 2025年1月11日
 *      Author: yongjie
 */

#ifndef BSP_GPT_H_
#define BSP_GPT_H_

#include "fsl_common.h"
#include "fsl_gpt.h"


#define EXAMPLE_GPT_CLOCK_SOURCE_SELECT (0U)
#define EXAMPLE_GPT_CLOCK_DIVIDER_SELECT (0U)

#define GPT1_DIVIDER 100
#define GPT2_DIVIDER 1

#define EXAMPLE_GPT1_CLK_FREQ ( (CLOCK_GetFreq(kCLOCK_IpgClk) / (EXAMPLE_GPT_CLOCK_DIVIDER_SELECT + 1U))/GPT1_DIVIDER )
#define COUNGER_NUMBER1  ((TIME_ms/1000.0)*EXAMPLE_GPT1_CLK_FREQ)
#define TIME_ms 1000

#define EXAMPLE_GPT2_CLK_FREQ ( (CLOCK_GetFreq(kCLOCK_IpgClk)\
      / (EXAMPLE_GPT_CLOCK_DIVIDER_SELECT + 1U))/GPT2_DIVIDER )
#define COUNGER_NUMBER2  ((TIME_ms/1000.0)*EXAMPLE_GPT2_CLK_FREQ)

#define EXAMPLE_GPT1 GPT1
#define EXAMPLE_GPT2 GPT2

#define GPT_IRQ_ID1 GPT1_IRQn
#define EXAMPLE_GPT1_IRQHandler GPT1_IRQHandler

#define GPT_IRQ_ID2 GPT2_IRQn
#define EXAMPLE_GPT2_IRQHandler GPT2_IRQHandler

typedef struct
{
	uint8_t   Capture_FinishFlag;   // 捕获结束标志位
	uint8_t   Capture_StartFlag;    // 捕获开始标志位
	uint32_t  Capture_CcrValue_1;     // 捕获寄存器的值
  uint32_t   Capture_CcrValue_2;     // 捕获寄存器的值
	uint16_t  Capture_Period;       // 定时器溢出次数
}GPT_ICUserValueTypeDef;

void GPT_Config(void);

#endif /* BSP_GPT_H_ */
