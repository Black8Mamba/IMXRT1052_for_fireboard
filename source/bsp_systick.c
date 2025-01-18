/*
 * bsp_systick.c
 *
 *  Created on: 2024年12月21日
 *      Author: yongjie
 */


/**
  ******************************************************************************
  * @file    bsp_systick.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   systickº¯Êý½Ó¿Ú
  ******************************************************************************
  * @attention
  *
  * ÊµÑéÆ½Ì¨:Ò°»ð  i.MXRT1052¿ª·¢°å
  * ÂÛÌ³    :http://www.firebbs.cn
  * ÌÔ±¦    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "bsp_systick.h"
#include <stdint.h>
#include "perf_counter/perf_counter.h"

static uint64_t sys_tick = 0;

/*==================================ÖÐ¶Ï·½Ê½====================================*/
static __IO uint32_t TimingDelay;

/**
  * @brief  Æô¶¯ÏµÍ³µÎ´ð¶¨Ê±Æ÷ SysTick
  * @param  ÎÞ
  * @retval ÎÞ
  */
void SysTick_Init(void)
{
  /* SystemFrequency / 1000    1msÖÐ¶ÏÒ»´Î
   * SystemFrequency / 100000  10usÖÐ¶ÏÒ»´Î
   * SystemFrequency / 1000000 1usÖÐ¶ÏÒ»´Î
   */
  if (SysTick_Config(SystemCoreClock / 1000))
  {
    /* ´íÎó´¦Àí */
    while (1);
  }
  NVIC_EnableIRQ(SysTick_IRQn);
}

/**
  * @brief   msÑÓÊ±³ÌÐò
  * @param
  *   @arg nTime: ÐèÒªÑÓÊ±¶àÉÙms
  * @retval  ÎÞ
  */
//void Delay_ms(__IO uint32_t nTime)
//{
//  TimingDelay = nTime;
//
//  /* TimingDelayÊÇÒ»¸öÈ«¾Ö±äÁ¿£¬ÔÚSysTickÖÐ¶Ï·þÎñº¯ÊýµÝ¼õ */
//  while(TimingDelay != 0);
//}

/**********************ÖÐ¶Ï·þÎñº¯Êý******************************/

/**
  * @brief  SysTickÖÐ¶Ï·þÎñº¯Êý
  * @param  ÎÞ
  * @retval ÎÞ
  * @attention
  */

void SysTick_Handler(void)
{
	sys_tick++;
	perfc_port_insert_to_system_timer_insert_ovf_handler();
}

#include <cr_section_macros.h>
//__RAMFUNC(RAM)
uint64_t get_sys_tick(void)
{
	return sys_tick;
}

/*==================================²éÑ¯·½Ê½==================================*/
 /*
  * couter ¼õ1µÄÊ±¼ä µÈÓÚ 1/systick_clk
  * µ±counter ´Ó reload µÄÖµ¼õÐ¡µ½0µÄÊ±ºò£¬ÎªÒ»¸öÑ­»·£¬Èç¹û¿ªÆôÁËÖÐ¶ÏÔòÖ´ÐÐÖÐ¶Ï·þÎñ³ÌÐò£¬
  * Í¬Ê± CTRL µÄ countflag Î»»áÖÃ1
  * ÕâÒ»¸öÑ­»·µÄÊ±¼äÎª reload * (1/systick_clk)
  */

/**
  * @brief  SysTick usÑÓÊ±º¯Êý
  * @param  ÎÞ
  *   @arg us: ÐèÒªÑÓÊ±¶àÉÙus
  * @retval ÎÞ
  * @attention
  */
void SysTick_Delay_Us( __IO uint32_t us)
{
  uint32_t i;
  SysTick_Config(SystemCoreClock/1000000);

  for(i=0;i<us;i++)
  {
    /* µ±¼ÆÊýÆ÷µÄÖµ¼õÐ¡µ½0µÄÊ±ºò£¬CRTL¼Ä´æÆ÷µÄÎ»16»áÖÃ1 */
    while( !((SysTick->CTRL)&(1<<16)) );
  }
  /* ¹Ø±ÕSysTick¶¨Ê±Æ÷ */
  SysTick->CTRL &=~SysTick_CTRL_ENABLE_Msk;
}

/**
  * @brief  SysTick msÑÓÊ±º¯Êý
  * @param  ÎÞ
  *   @arg ms: ÐèÒªÑÓÊ±¶àÉÙms
  * @retval ÎÞ
  * @attention
  */
void SysTick_Delay_Ms( __IO uint32_t ms)
{
	uint64_t tick_target = sys_tick+ms;
	while(sys_tick < tick_target);
}


/*********************************************END OF FILE**********************/
