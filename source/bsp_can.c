/*
 * bsp_can.c
 *
 *  Created on: 2025年1月19日
 *      Author: yongjie
 */

#include "fsl_debug_console.h"
#include "bsp_can.h"
#include "bsp_can.h"

#include "elog.h"

volatile bool rxComplete = false;
flexcan_frame_t txFrame, rxFrame;

void CAN_Mode_Config(uint32_t baudRate, bool LoopBack)
{
	/**************** 第一部分 *************************/
	/*CAN 配置结构体 */
	flexcan_config_t flexcanConfig; //FlexCAN 模块配置结构

	/******************** 第二部分 ********************/
	/*Clock setting for FLEXCAN*/
	CLOCK_SetMux(kCLOCK_CanMux, FLEXCAN_CLOCK_SOURCE_SELECT);
	CLOCK_SetDiv(kCLOCK_CanDiv, FLEXCAN_CLOCK_SOURCE_DIVIDER);

	/******************* 第三部分 *********************/
	/* 初始化 FlexCAN . */
	/*
	 * flexcanConfig.clkSrc = kFLEXCAN_ClkSrcOsc;
	 * flexcanConfig.baudRate = 1000000U; //设置波特率为 1 Mbps
	 * flexcanConfig.baudRateFD = 2000000U; //柔性版 FD 波特率在 bps
	 * flexcanConfig.maxMbNum = 16; //用户使用的消息缓冲区的最大数量
	 * flexcanConfig.enableLoopBack = false; //禁用回环自检模式
	 * flexcanConfig.enableSelfWakeup = false; //启用或禁用自唤醒模式
	 * flexcanConfig.enableIndividMask = false;//启用或禁用 Rx 单个掩码
	 * flexcanConfig.enableDoze = false; //启用或禁用 doze 模式
	 */
	FLEXCAN_GetDefaultConfig(&flexcanConfig);

	flexcanConfig.enableLoopBack = LoopBack;//设置为回环模式
	flexcanConfig.baudRate = baudRate;
	FLEXCAN_Init(EXAMPLE_CAN, &flexcanConfig, EXAMPLE_CAN_CLK_FREQ);
}

/**
* @brief  ³õÊ¼»¯CAN½ÓÊÕ»º³åÇø
*/
void CAN_RX_Buffer_Config(uint32_t ID_STD,uint8_t RX_MB)
{
  flexcan_rx_mb_config_t mbConfig;


  mbConfig.format = kFLEXCAN_FrameFormatStandard;
  mbConfig.type = kFLEXCAN_FrameTypeData;
  mbConfig.id = FLEXCAN_ID_STD(ID_STD);

  FLEXCAN_SetRxMbConfig(EXAMPLE_CAN, RX_MB, &mbConfig, true);

   /* Enable Rx Message Buffer interrupt. */
  FLEXCAN_EnableMbInterrupts(EXAMPLE_CAN, 1 << RX_MB);
  EnableIRQ(EXAMPLE_FLEXCAN_IRQn);
}

/**
* @brief  ³õÊ¼»¯CAN·¢ËÍ»º³åÇø
*/
void CAN_TX_Buffer_Config(uint32_t ID_STD)
{
  txFrame.format = kFLEXCAN_FrameFormatStandard;
  txFrame.type = kFLEXCAN_FrameTypeData;
  txFrame.id = FLEXCAN_ID_STD(ID_STD);
  txFrame.length = DLC;

  txFrame.dataWord0 = CAN_WORD0_DATA_BYTE_0(0x11) | CAN_WORD0_DATA_BYTE_1(0x22) | CAN_WORD0_DATA_BYTE_2(0x33) |
  CAN_WORD0_DATA_BYTE_3(0x44);
  txFrame.dataWord1 = CAN_WORD1_DATA_BYTE_4(0x55) | CAN_WORD1_DATA_BYTE_5(0x66) | CAN_WORD1_DATA_BYTE_6(0x77) |
  CAN_WORD1_DATA_BYTE_7(0x88);
}

/**
* @brief  CAN³õÊ¼»¯
*/
void CAN_Config(void)
{
  CAN_Mode_Config(1000000U,1);
}

/**
* @brief  CAN½ÓÊÕÖÐ¶Ï·þÎñº¯Êý
*/
void EXAMPLE_FLEXCAN_IRQHandler(void)
{
    /* If new data arrived. */
    if (FLEXCAN_GetMbStatusFlags(EXAMPLE_CAN, 1 << RX_MESSAGE_BUFFER_NUM))
    {
        FLEXCAN_ClearMbStatusFlags(EXAMPLE_CAN, 1 << RX_MESSAGE_BUFFER_NUM);

        FLEXCAN_ReadRxMb(EXAMPLE_CAN, RX_MESSAGE_BUFFER_NUM, &rxFrame);

        rxComplete = true;
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void loop_test(void)
{
	CAN_RX_Buffer_Config(0x123,RX_MESSAGE_BUFFER_NUM);
	FLEXCAN_SetTxMbConfig(EXAMPLE_CAN, TX_MESSAGE_BUFFER_NUM, true);
	CAN_TX_Buffer_Config(0x123);
    log_i("send from MB:%d to MB:%d", TX_MESSAGE_BUFFER_NUM, RX_MESSAGE_BUFFER_NUM);


    log_i("tx word0 = 0x%x\r\n", txFrame.dataWord0);
    log_i("tx word1 = 0x%x\r\n", txFrame.dataWord1);

    FLEXCAN_TransferSendBlocking(EXAMPLE_CAN, TX_MESSAGE_BUFFER_NUM, &txFrame);

    while (!rxComplete)
    {
    }

    rxComplete = false;

    log_i("receive data from:%d\n", RX_MESSAGE_BUFFER_NUM);

    log_i("rx word0 = 0x%x\n", rxFrame.dataWord0);
    log_i("rx word1 = 0x%x\n", rxFrame.dataWord1);

//    FLEXCAN_DisableMbInterrupts(EXAMPLE_CAN, 1 << RX_MESSAGE_BUFFER_NUM);

    log_i("loop test done!\n");
}


