/*
 * bsp_gpt.c
 *
 *  Created on: 2025年1月11日
 *      Author: yongjie
 */


#include "bsp_nvic.h"
#include "bsp_gpt.h"
#include "bsp_led.h"
#include "elog.h"

unsigned int i = 0;

const gpt_config_t GPT1_config = {
  .clockSource = kGPT_ClockSource_Periph,
  .divider = 1UL,
  .enableFreeRun = false,
  .enableRunInWait = true,
  .enableRunInStop = true,
  .enableRunInDoze = false,
  .enableRunInDbg = false,
  .enableMode = true
};

void GPT_Config(void)
{
//  gpt_config_t gptConfig;

  log_i("kCLOCK_PerClk:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_PerClk));

  /* GPT device and channels initialization */
  GPT_Init(EXAMPLE_GPT1, &GPT1_config);
  GPT_SetOscClockDivider(EXAMPLE_GPT1, 1);
  GPT_SetOutputCompareValue(EXAMPLE_GPT1, kGPT_OutputCompare_Channel1, COUNGER_NUMBER1);
  GPT_SetOutputOperationMode(EXAMPLE_GPT1, kGPT_OutputCompare_Channel1, kGPT_OutputOperation_Disconnected);
  /* Enable GPT interrupt sources */
  GPT_EnableInterrupts(EXAMPLE_GPT1, kGPT_OutputCompare1InterruptEnable);
  /* Enable interrupt GPT1_IRQn request in the NVIC. */
  EnableIRQ(GPT_IRQ_ID1);
  /* Start the GPT timer */
  GPT_StartTimer(EXAMPLE_GPT1);
}

void EXAMPLE_GPT1_IRQHandler(void)
{
  GPT_ClearStatusFlags(EXAMPLE_GPT1, kGPT_OutputCompare1Flag);
  i++;

  if(i%2)
  {
//	  CORE_BOARD_LED(1);
  }
  else
  {
//	  CORE_BOARD_LED(0);
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
