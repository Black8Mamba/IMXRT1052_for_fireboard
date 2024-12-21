/*
 * bsp_systick.h
 *
 *  Created on: 2024年12月21日
 *      Author: yongjie
 */

#ifndef BSP_SYSTICK_H_
#define BSP_SYSTICK_H_

#include "fsl_common.h"

void SysTick_Init(void);
void Delay_ms(__IO uint32_t nTime);


void SysTick_Delay_Us( __IO uint32_t us);
void SysTick_Delay_Ms( __IO uint32_t ms);
uint64_t get_sys_tick(void);

#endif /* BSP_SYSTICK_H_ */
