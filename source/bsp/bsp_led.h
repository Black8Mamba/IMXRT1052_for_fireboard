/*
 * bsp_led.h
 *
 *  Created on: 2025年3月29日
 *      Author: yongjie
 */

#ifndef BSP_BSP_LED_H_
#define BSP_BSP_LED_H_

#include "fsl_iomuxc.h"
#include "fsl_gpio.h"

#define ON 1
#define OFF 0

#define CORE_BOARD_LED_GPIO             GPIO1
#define CORE_BOARD_LED_GPIO_PIN         (9U)
#define CORE_BOARD_LED_IOMUXC           IOMUXC_GPIO_AD_B0_09_GPIO1_IO09

#define RGB_RED_LED_GPIO                GPIO1
#define RGB_RED_LED_GPIO_PIN            (24U)
#define RGB_RED_LED_IOMUXC              IOMUXC_GPIO_AD_B1_08_GPIO1_IO24

#define RGB_GREEN_LED_GPIO              GPIO1
#define RGB_GREEN_LED_GPIO_PIN          (25U)
#define RGB_GREEN_LED_IOMUXC            IOMUXC_GPIO_AD_B1_09_GPIO1_IO25

#define RGB_BLUE_LED_GPIO               GPIO1
#define RGB_BLUE_LED_GPIO_PIN           (10U)
#define RGB_BLUE_LED_IOMUXC             IOMUXC_GPIO_AD_B0_10_GPIO1_IO10

#define   CORE_BOARD_LED(a) if (a)  \
          GPIO_PinWrite(CORE_BOARD_LED_GPIO, CORE_BOARD_LED_GPIO_PIN, 0U);\
          else    \
          GPIO_PinWrite(CORE_BOARD_LED_GPIO, CORE_BOARD_LED_GPIO_PIN, 1U);

#define   RGB_RED_LED(a)  if (a)  \
          GPIO_PinWrite(RGB_RED_LED_GPIO, RGB_RED_LED_GPIO_PIN, 0U);\
          else    \
          GPIO_PinWrite(RGB_RED_LED_GPIO, RGB_RED_LED_GPIO_PIN, 1U);

#define   RGB_GREEN_LED(a)  if (a)  \
          GPIO_PinWrite(RGB_GREEN_LED_GPIO, RGB_GREEN_LED_GPIO_PIN, 0U);\
          else    \
          GPIO_PinWrite(RGB_GREEN_LED_GPIO, RGB_GREEN_LED_GPIO_PIN, 1U);

#define   RGB_BLUE_LED(a) if (a)  \
          GPIO_PinWrite(RGB_BLUE_LED_GPIO, RGB_BLUE_LED_GPIO_PIN, 0U);\
          else    \
          GPIO_PinWrite(RGB_BLUE_LED_GPIO, RGB_BLUE_LED_GPIO_PIN, 1U);

//red
#define RGB_LED_COLOR_RED \
do { \
	RGB_RED_LED(ON); \
	RGB_GREEN_LED(OFF); \
	RGB_BLUE_LED(OFF); \
}while(0);

//green
#define RGB_LED_COLOR_GREEN \
do { \
	RGB_RED_LED(OFF); \
	RGB_GREEN_LED(ON); \
	RGB_BLUE_LED(OFF); \
}while(0);

//blue
#define RGB_LED_COLOR_BLUE \
do { \
	RGB_RED_LED(OFF); \
	RGB_GREEN_LED(OFF); \
	RGB_BLUE_LED(ON); \
}while(0);

//yellow
#define RGB_LED_COLOR_YELLOW \
do { \
	RGB_RED_LED(ON); \
	RGB_GREEN_LED(ON); \
	RGB_BLUE_LED(OFF); \
}while(0);

//PURPLE
#define RGB_LED_COLOR_PURPLE \
do { \
	RGB_RED_LED(ON); \
	RGB_GREEN_LED(OFF); \
	RGB_BLUE_LED(ON); \
}while(0);

//CYAN
#define RGB_LED_COLOR_CYAN \
do { \
	RGB_RED_LED(OFF); \
	RGB_GREEN_LED(ON); \
	RGB_BLUE_LED(ON); \
}while(0);

//white
#define RGB_LED_COLOR_WHITE \
do { \
	RGB_RED_LED(ON); \
	RGB_GREEN_LED(ON); \
	RGB_BLUE_LED(ON); \
}while(0);

//all off
#define RGB_LED_COLOR_BLACK \
do { \
	RGB_RED_LED(OFF); \
	RGB_GREEN_LED(OFF); \
	RGB_BLUE_LED(OFF); \
}while(0);

#endif /* BSP_BSP_LED_H_ */
