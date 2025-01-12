/*
 * bsp_snvs_hp_rtc.c
 *
 *  Created on: 2025年1月12日
 *      Author: yongjie
 */

#include "bsp_snvs_hp_rtc.h"
#include "elog.h"

/***********************************************************************************************************************
 * SNVS initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'SNVS'
- type: 'snvs_hp'
- mode: 'general'
- custom_name_enabled: 'false'
- type_id: 'snvs_hp_5fc0a925779e7e5f8b9bedf272c8d94e'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'SNVS'
- config_sets:
  - fsl_snvs_hp:
    - snvs_hp_config:
      - init: 'true'
      - hac:
        - set: 'false'
        - initialValue: '0'
        - load: 'false'
        - enable: 'false'
        - start: 'false'
    - initRTC: 'true'
    - snvs_hp_rtc_config:
      - rtcCalEnable: 'true'
      - rtcCalValueInt: '0'
      - periodicInterruptFreq: '0'
      - start: 'true'
      - callTimeSynchronize: 'true'
      - setAlarmDateTime: 'true'
      - snvs_hp_rtc_alarm_datetime:
        - year: '2025'
        - month: '1'
        - day: '12'
        - hour: '20'
        - minute: '41'
        - second: '0'
    - interruptsCfg:
      - interruptSources: ''
      - isInterruptEnabled: 'false'
      - interrupt:
        - IRQn: 'SNVS_HP_WRAPPER_IRQn'
        - enable_interrrupt: 'enabled'
        - enable_priority: 'false'
        - priority: '0'
        - enable_custom_name: 'false'
      - isSecureInterruptEnabled: 'false'
      - secureInterrupt:
        - IRQn: 'SNVS_HP_WRAPPER_TZ_IRQn'
        - enable_interrrupt: 'enabled'
        - enable_priority: 'false'
        - priority: '0'
        - enable_custom_name: 'false'
      - isPulseInterruptEnabled: 'false'
      - pulseInterrupt:
        - IRQn: 'SNVS_LP_WRAPPER_IRQn'
        - enable_interrrupt: 'enabled'
        - enable_priority: 'false'
        - priority: '0'
        - enable_custom_name: 'false'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const snvs_hp_rtc_config_t SNVS_config = {
  .rtcCalEnable = true,
  .rtcCalValue = 0,
  .periodicInterruptFreq = 0
};
snvs_hp_rtc_datetime_t SNVS_alarmDateTimeStruct = {
  .year = 2025U,
  .month = 1U,
  .day = 12U,
  .hour = 20U,
  .minute = 41U,
  .second = 0U
};

void SNVS_init(void) {
  /* SNVS HP RTC initialization */
  SNVS_HP_RTC_Init(SNVS_PERIPHERAL, &SNVS_config);
  /* SNVS HP alarm date and time initialization */
  SNVS_HP_RTC_SetDatetime(SNVS_PERIPHERAL, &SNVS_alarmDateTimeStruct);
  /* Synchronize date and time */
//  SNVS_HP_RTC_TimeSynchronize(SNVS_PERIPHERAL);
  /* SNVS HP RTC start timer */
  SNVS_HP_RTC_StartTimer(SNVS_PERIPHERAL);
}

void RTC_TimeAndDate_Show(void)
{
	snvs_hp_rtc_datetime_t rtcDate;
	SNVS_HP_RTC_GetDatetime(SNVS_PERIPHERAL, &rtcDate);
	log_i("The Date : Y:%0.2d - M:%0.2d - D:%0.2d\r\n", rtcDate.year, rtcDate.month, rtcDate.day);
}

