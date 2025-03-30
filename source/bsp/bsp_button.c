/*
 * bsp_button.c
 *
 *  Created on: 2025年3月30日
 *      Author: yongjie
 */

#include "fsl_component_button.h"
#include "elog.h"

#undef LOG_TAG
#undef LOG_LVL
#define LOG_TAG    "button"
#define LOG_LVL    ELOG_LVL_DEBUG

static BUTTON_HANDLE_ARRAY_DEFINE(s_buttonArrayHandle, 2);

button_status_t button_callback(void *buttonHandle,
                                             button_callback_message_t *message,
                                             void *callbackParam)
{
	log_i("button_callback");

	return kStatus_BUTTON_Success;
}

 void button_test(void)
 {
	 button_config_t buttonArrayConfig[2];
	 buttonArrayConfig[0].gpio.port = 5;
	 buttonArrayConfig[0].gpio.pin = 0;
	 buttonArrayConfig[0].gpio.direction = 0;
	 buttonArrayConfig[0].gpio.pinStateDefault = 0;

	 BUTTON_Init((button_handle_t)s_buttonArrayHandle[0], &buttonArrayConfig[0]);

	 buttonArrayConfig[1].gpio.port = 1;
	 buttonArrayConfig[1].gpio.pin = 4;
	 buttonArrayConfig[1].gpio.pinStateDefault = 1;
	 buttonArrayConfig[1].gpio.direction = 0;
	 BUTTON_Init((button_handle_t)s_buttonArrayHandle[1], &buttonArrayConfig[1]);

	 button_status_t status = BUTTON_InstallCallback((button_handle_t)s_buttonArrayHandle[0], button_callback, NULL);
	 if (status != kStatus_BUTTON_Success)
	 {
		 log_e("BUTTON_InstallCallback failed!");
		 return;
	 }

	 status = BUTTON_InstallCallback((button_handle_t)s_buttonArrayHandle[1], button_callback, NULL);
	 if (status != kStatus_BUTTON_Success)
	 {
		 log_e("BUTTON_InstallCallback failed!");
		 return;
	 }
 }

