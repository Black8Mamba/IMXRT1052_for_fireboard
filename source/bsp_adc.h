/*
 * bsp_adc.h
 *
 *  Created on: 2025年1月12日
 *      Author: yongjie
 */

#ifndef BSP_ADC_H_
#define BSP_ADC_H_

#include "fsl_adc.h"
#include "fsl_adc_etc.h"

/* BOARD_InitPeripherals defines for ADC1 */
/* Definition of peripheral ID */
#define ADC1_PERIPHERAL ADC1
/* Definition of special channel interconnected with ADC_ETC which takes real channel to be measured from ADC_ETC. */
#define ADC1_CHANNEL_DRIVEN_BY_ADC_ETC 16U
/* ADC1 interrupt vector ID (number). */
#define ADC1_IRQN ADC1_IRQn
/* ADC1 interrupt handler identifier. */
#define ADC1_IRQHANDLER ADC1_IRQHandler
/* Channel 0 (IN.0) conversion control group. */
#define ADC1_CH0_CONTROL_GROUP 0U

void ADC1_init(void);

#endif /* BSP_ADC_H_ */
