/*
 * bsp_led.c
 *
 *  Created on: 2025年3月29日
 *      Author: yongjie
 */

#include "fsl_component_led.h"
#include "fsl_gpio.h"

//LED_HANDLE_DEFINE(s_ledRgbHandle);
//static led_config_t ledRgbConfig;
void led_test(void)
{
//	led_config_t ledMonochromeConfig;
//	ledMonochromeConfig.type = kLED_TypeMonochrome;
//	ledMonochromeConfig.ledMonochrome.monochromePin.dimmingEnable = 0;
//	ledMonochromeConfig.ledMonochrome.monochromePin.gpio.port = 0;
//	ledMonochromeConfig.ledMonochrome.monochromePin.gpio.pin = 1;
//	ledMonochromeConfig.ledMonochrome.monochromePin.gpio.pinStateDefault = 0;
//	LED_Init((led_handle_t)s_ledMonochromeHandle, &ledMonochromeConfig);

//	ledRgbConfig.type = kLED_TypeRgb;
//	ledRgbConfig.ledRgb.redPin.dimmingEnable = 0;
//	ledRgbConfig.ledRgb.redPin.gpio.port = 1;
//	ledRgbConfig.ledRgb.redPin.gpio.pin = 24;
//	ledRgbConfig.ledRgb.redPin.gpio.direction = kHAL_GpioDirectionOut;
//	ledRgbConfig.ledRgb.redPin.gpio.level = 0;
//	ledRgbConfig.ledRgb.greenPin.dimmingEnable = 0;
//	ledRgbConfig.ledRgb.greenPin.gpio.port = 1;
//	ledRgbConfig.ledRgb.greenPin.gpio.pin = 25;
//	ledRgbConfig.ledRgb.greenPin.gpio.direction = kHAL_GpioDirectionOut;
//	ledRgbConfig.ledRgb.greenPin.gpio.level = 0;
//	ledRgbConfig.ledRgb.bluePin.dimmingEnable = 0;
//	ledRgbConfig.ledRgb.bluePin.gpio.port = 1;
//	ledRgbConfig.ledRgb.bluePin.gpio.pin = 10;
//	ledRgbConfig.ledRgb.bluePin.gpio.direction = kHAL_GpioDirectionOut;
//	ledRgbConfig.ledRgb.bluePin.gpio.level = 0;
//	LED_Init((led_handle_t)s_ledRgbHandle, &ledRgbConfig);
//	LED_SetColor(s_ledRgbHandle, kLED_Red);
}
