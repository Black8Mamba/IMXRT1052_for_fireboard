/*
 * bsp_can.h
 *
 *  Created on: 2025年1月19日
 *      Author: yongjie
 */

#ifndef BSP_CAN_H_
#define BSP_CAN_H_

#include "fsl_common.h"
#include "fsl_flexcan.h"

#define EXAMPLE_CAN CAN2 //定义使用的 CAN
#define EXAMPLE_FLEXCAN_IRQn CAN2_IRQn //定义中断号
#define EXAMPLE_FLEXCAN_IRQHandler CAN2_IRQHandler//定义中断服务函数

#define RX_MESSAGE_BUFFER_NUM (9) //定义使用的接收邮箱
#define TX_MESSAGE_BUFFER_NUM (8) //定义使用的发送邮箱
#define DLC (8) //定义数据长度

#define CAN_RX_ID 0x123
#define CAN_TX_ID 0x123

//#define SET_CAN_QUANTUM 0
//#define PSEG1 3
//#define PSEG2 2
//#define PROPSEG 1

/* 时钟相关宏定义 */
/* 选择时钟源，PLL3(480 MHz) 经过 6 分频后 (80MHz) 作为 CAN 根时钟。*/
#define FLEXCAN_CLOCK_SOURCE_SELECT (2U)
/* 设置时钟分频，80MHz 的 CAN 根时钟经过分频后作为 CAN 时钟源。*/
#define FLEXCAN_CLOCK_SOURCE_DIVIDER (3U)
/* 读取 CAN 是工作频率 */
#define EXAMPLE_CAN_CLK_FREQ ((CLOCK_GetFreq(kCLOCK_Usb1PllClk) / 6)\
		/ (FLEXCAN_CLOCK_SOURCE_DIVIDER + 1U))

void CAN_Mode_Config(uint32_t baudRate, bool LoopBack);
void CAN_Config(void);
void CAN_RX_Buffer_Config(uint32_t ID_STD,uint8_t RX_MB);
void CAN_TX_Buffer_Config(uint32_t ID_STD);
void loop_test(void);

#endif /* BSP_CAN_H_ */
