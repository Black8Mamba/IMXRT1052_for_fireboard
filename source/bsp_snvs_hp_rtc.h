/*
 * bsp_snvs_hp_rtc.h
 *
 *  Created on: 2025年1月12日
 *      Author: yongjie
 */

#ifndef BSP_SNVS_HP_RTC_H_
#define BSP_SNVS_HP_RTC_H_

#include "fsl_snvs_hp.h"

/* Definition of peripheral ID */
#define SNVS_PERIPHERAL SNVS

/* SNVS HP configuration */
extern const snvs_hp_rtc_config_t SNVS_config;
/* SNVS HP alarm date and time structure */
extern snvs_hp_rtc_datetime_t SNVS_alarmDateTimeStruct;

void SNVS_init(void);
void RTC_TimeAndDate_Show(void);

#endif /* BSP_SNVS_HP_RTC_H_ */
