/*
 * bsp_qtmr.h
 *
 *  Created on: 2025年1月11日
 *      Author: yongjie
 */

#ifndef BSP_QTMR_H_
#define BSP_QTMR_H_

#include "fsl_common.h"
#include "fsl_qtmr.h"

#define BOARD_QTMR_BASEADDR TMR3
#define BOARD_FIRST_QTMR_CHANNEL kQTMR_Channel_0
#define BOARD_SECOND_QTMR_CHANNEL kQTMR_Channel_1
#define QTMR_ClockCounterOutput kQTMR_ClockCounter0Output

#define QTMR_IRQ_ID TMR3_IRQn
#define QTMR_IRQ_HANDLER TMR3_IRQHandler

#define QTMR_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_IpgClk)

#define TMR_TIMIER 50

#endif /* BSP_QTMR_H_ */
