/*
 * bsp_qtmr.c
 *
 *  Created on: 2025年1月11日
 *      Author: yongjie
 */

#include "bsp_nvic.h"
#include "bsp_qtmr.h"
#include "bsp_led.h"

volatile uint32_t qtmrIsrFlag = 0;

void TMR_Init(void)
{

  qtmr_config_t qtmrConfig;

  QTMR_GetDefaultConfig(&qtmrConfig);
  qtmrConfig.primarySource = kQTMR_ClockDivide_128;
  QTMR_Init(BOARD_QTMR_BASEADDR, BOARD_SECOND_QTMR_CHANNEL, &qtmrConfig);

  QTMR_SetTimerPeriod(BOARD_QTMR_BASEADDR, BOARD_SECOND_QTMR_CHANNEL, MSEC_TO_COUNT(TMR_TIMIER, (QTMR_SOURCE_CLOCK / 128)));

  QTMR_EnableInterrupts(BOARD_QTMR_BASEADDR, BOARD_SECOND_QTMR_CHANNEL, kQTMR_CompareInterruptEnable);

  set_IRQn_Priority(QTMR_IRQ_ID,Group4_PreemptPriority_6, Group4_SubPriority_0);
  EnableIRQ(QTMR_IRQ_ID);

  QTMR_StartTimer(BOARD_QTMR_BASEADDR, BOARD_SECOND_QTMR_CHANNEL, kQTMR_PriSrcRiseEdge);
}

void QTMR_IRQ_HANDLER(void)
{
    QTMR_ClearStatusFlags(BOARD_QTMR_BASEADDR,\
        BOARD_SECOND_QTMR_CHANNEL, kQTMR_CompareFlag);
    qtmrIsrFlag++;
    if((qtmrIsrFlag % 20 == 0))
    {
//        	log_i("qtimer:%lld", get_system_ms());
    	RGB_LED_COLOR_YELLOW
    }
    else
    {
//    	RGB_LED_COLOR_PURPLE
    }
}
