/*
 * bsp_adc.c
 *
 *  Created on: 2025年1月12日
 *      Author: yongjie
 */

#include "bsp_adc.h"

/***********************************************************************************************************************
 * ADC1 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'ADC1'
- type: 'adc_12b1msps_sar'
- mode: 'ADC_GENERAL'
- custom_name_enabled: 'false'
- type_id: 'adc_12b1msps_sar_6a490e886349a7b2b07bed10ce7b299b'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'ADC1'
- config_sets:
  - fsl_adc:
    - clockConfig:
      - clockSource: 'kADC_ClockSourceAD'
      - clockSourceFreq: 'custom:10 MHz'
      - clockDriver: 'kADC_ClockDriver1'
      - samplePeriodMode: 'kADC_SamplePeriodLong16Clcoks'
      - enableAsynchronousClockOutput: 'false'
    - conversionConfig:
      - resolution: 'kADC_Resolution12Bit'
      - hardwareAverageMode: 'kADC_HardwareAverageCount32'
      - enableHardwareTrigger: 'software'
      - enableHighSpeed: 'false'
      - enableLowPower: 'false'
      - enableContinuousConversion: 'false'
      - enableOverWrite: 'false'
      - enableDma: 'false'
    - resultingTime: []
    - resultCorrection:
      - doAutoCalibration: 'false'
      - offset: '0'
    - hardwareCompareConfiguration:
      - hardwareCompareMode: 'disabled'
      - value1: '0'
      - value2: '0'
    - enableInterrupt: 'true'
    - adc_interrupt:
      - IRQn: 'ADC1_IRQn'
      - enable_interrrupt: 'enabled'
      - enable_priority: 'false'
      - priority: '0'
      - enable_custom_name: 'false'
    - adc_channels_config:
      - 0:
        - channelNumber: 'IN.0'
        - channelName: ''
        - channelGroup: '0'
        - initializeChannel: 'true'
        - enableInterruptOnConversionCompleted: 'true'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const adc_config_t ADC1_config = {
  .enableOverWrite = false,
  .enableContinuousConversion = false,
  .enableHighSpeed = false,
  .enableLowPower = false,
  .enableLongSample = true,
  .enableAsynchronousClockOutput = true,
  .referenceVoltageSource = kADC_ReferenceVoltageSourceAlt0,
  .samplePeriodMode = kADC_SamplePeriodLong16Clcoks,
  .clockSource = kADC_ClockSourceAD,
  .clockDriver = kADC_ClockDriver1,
  .resolution = kADC_Resolution12Bit
};
const adc_channel_config_t ADC1_channels_config[1] = {
  {
    .channelNumber = 0U,
    .enableInterruptOnConversionCompleted = true
  }
};

void ADC1_init(void) {
  /* Initialize ADC1 peripheral. */
  ADC_Init(ADC1_PERIPHERAL, &ADC1_config);
  /* Configure ADC1 peripheral to average 32 conversions in one measurement. */
  ADC_SetHardwareAverageConfig(ADC1_PERIPHERAL, kADC_HardwareAverageCount32);
  /* Initialize ADC1 channel 0. */
  ADC_SetChannelConfig(ADC1_PERIPHERAL, ADC1_CH0_CONTROL_GROUP, &ADC1_channels_config[0]);
  /* Enable interrupt ADC1_IRQn request in the NVIC. */
  EnableIRQ(ADC1_IRQN);
}

volatile bool ADC_ConversionDoneFlag;
volatile uint32_t ADC_ConvertedValue;

void ADC1_IRQHANDLER(void)
{
	ADC_ConversionDoneFlag = true;
	ADC_ConvertedValue = ADC_GetChannelConversionValue(ADC1_PERIPHERAL, ADC1_CH0_CONTROL_GROUP);
}



