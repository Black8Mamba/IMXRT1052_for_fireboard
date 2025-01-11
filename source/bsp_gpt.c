/*
 * bsp_gpt.c
 *
 *  Created on: 2025年1月11日
 *      Author: yongjie
 */


#include "bsp_nvic.h"
#include "bsp_gpt.h"
#include "bsp_led.h"

unsigned int i = 0;

void GPT_Config(void)
{
  gpt_config_t gptConfig;

  CLOCK_SetMux(kCLOCK_PerclkMux, EXAMPLE_GPT_CLOCK_SOURCE_SELECT);
  CLOCK_SetDiv(kCLOCK_PerclkDiv, EXAMPLE_GPT_CLOCK_DIVIDER_SELECT);

  GPT_GetDefaultConfig(&gptConfig);
  gptConfig.enableFreeRun = false;
  GPT_Init(EXAMPLE_GPT1, &gptConfig);
  GPT_Init(EXAMPLE_GPT2, &gptConfig);

  GPT_SetClockDivider(EXAMPLE_GPT1, GPT1_DIVIDER);
  GPT_SetClockDivider(EXAMPLE_GPT2, GPT2_DIVIDER);

  GPT_SetOutputCompareValue(EXAMPLE_GPT1, kGPT_OutputCompare_Channel1, (uint32_t)COUNGER_NUMBER1);

  GPT_EnableInterrupts(EXAMPLE_GPT1, kGPT_OutputCompare1InterruptEnable);

  set_IRQn_Priority(GPT_IRQ_ID1,Group4_PreemptPriority_6, Group4_SubPriority_0);
  EnableIRQ(GPT_IRQ_ID1);

  GPT_StartTimer(EXAMPLE_GPT1);


  GPT_SetInputOperationMode(EXAMPLE_GPT2,kGPT_InputCapture_Channel2,kGPT_InputOperation_FallEdge);
  GPT_EnableInterrupts(EXAMPLE_GPT2, kGPT_InputCapture2InterruptEnable);
  GPT_EnableInterrupts(EXAMPLE_GPT2,kGPT_RollOverFlagInterruptEnable);

  set_IRQn_Priority(GPT_IRQ_ID2,Group4_PreemptPriority_6, Group4_SubPriority_0);
  EnableIRQ(GPT_IRQ_ID2);

  GPT_StartTimer(EXAMPLE_GPT2);
}


void EXAMPLE_GPT1_IRQHandler(void)
{
  GPT_ClearStatusFlags(EXAMPLE_GPT1, kGPT_OutputCompare1Flag);
  i++;

  if(i%2)
  {
	  CORE_BOARD_LED(1);
  }
  else
  {
	  CORE_BOARD_LED(0);
  }
}

volatile GPT_ICUserValueTypeDef GPT_ICUserValueStructure = {0,0,0,0,0};

void EXAMPLE_GPT2_IRQHandler(void)
{
   if ( GPT_GetStatusFlags(EXAMPLE_GPT2,kGPT_RollOverFlag) != false )
   {
      if ( GPT_ICUserValueStructure.Capture_StartFlag != 0 )
      {
        GPT_ICUserValueStructure.Capture_Period ++;
      }
      GPT_ClearStatusFlags(EXAMPLE_GPT2, kGPT_RollOverFlag);
   }

  if (GPT_GetStatusFlags(EXAMPLE_GPT2,kGPT_InputCapture2Flag) != false)
  {

      if(GPT_ICUserValueStructure.Capture_FinishFlag != 1)
      {
        if ( GPT_ICUserValueStructure.Capture_StartFlag == 0 )
        {
          GPT_ICUserValueStructure.Capture_Period = 0;

          GPT_ICUserValueStructure.Capture_CcrValue_1 = GPT_GetInputCaptureValue(EXAMPLE_GPT2,kGPT_InputCapture_Channel2);
          GPT_SetInputOperationMode(EXAMPLE_GPT2,kGPT_InputCapture_Channel2,kGPT_InputOperation_RiseEdge);
          GPT_ICUserValueStructure.Capture_StartFlag = 1;
        }
        else
        {
          GPT_ICUserValueStructure.Capture_CcrValue_2 = GPT_GetInputCaptureValue(EXAMPLE_GPT2,kGPT_InputCapture_Channel2);
          GPT_SetInputOperationMode(EXAMPLE_GPT2,kGPT_InputCapture_Channel2,kGPT_InputOperation_FallEdge);
          GPT_ICUserValueStructure.Capture_StartFlag = 0;
          GPT_ICUserValueStructure.Capture_FinishFlag = 1;
         }
       }

     GPT_ClearStatusFlags(EXAMPLE_GPT2, kGPT_InputCapture2Flag);
  }

}
