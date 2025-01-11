/*
 * bsp_nvic.c
 *
 *  Created on: 2025年1月11日
 *      Author: yongjie
 */


#include "bsp_nvic.h"

void Set_NVIC_PriorityGroup(PriorityGroup_Type PriorityGroup)
{
  NVIC_SetPriorityGrouping((uint32_t)PriorityGroup);
}

void set_IRQn_Priority(IRQn_Type IRQn,PreemptPriority_Type PreemptPriority, SubPriority_Type SubPriorit)
{
  uint32_t PriorityGroup = 0;
  uint32_t Priority_Encode = 0;
  PriorityGroup = NVIC_GetPriorityGrouping();
  Priority_Encode = NVIC_EncodePriority (PriorityGroup,(uint32_t)PreemptPriority,(uint32_t)SubPriorit);

  NVIC_SetPriority(IRQn, Priority_Encode);
}
