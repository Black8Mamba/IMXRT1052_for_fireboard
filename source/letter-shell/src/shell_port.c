/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#include "shell.h"
#include "fsl_debug_console.h"
//#include "easyflash.h"
#include "elog.h"
#include "fsl_adapter_rng.h"
#include "fsl_component_log.h"
#include "fsl_debug_console.h"
#include "FreeRtos.h"
#include "task.h"
#include "lwrb/lwrb.h"
//#include "bsp_phy.h"
Shell shell;
char shellBuffer[512];

lwrb_t shell_rb;
static uint8_t uart_buffer[512];

/**
 * @brief 用户shell写
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际写入的数据长度
 */
int DbgConsole_SendDataReliable(uint8_t *ch, size_t size);
short userShellWrite(char *data, unsigned short len)
{
//    serialTransmit(&debugSerial, (uint8_t *)data, len, 0x1FF);
//    return len;
	return DbgConsole_SendDataReliable((uint8_t *)data, (size_t)len);
}


/**
 * @brief 用户shell读
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际读取到
 */
int DbgConsole_Getchar(void);
short userShellRead(char *data, unsigned short len)
{
	char c = DbgConsole_Getchar();
	*data = c;
	return 1;
}

/**
 * @brief 用户shell上锁
 * 
 * @param shell shell
 * 
 * @return int 0
 */
//int userShellLock(Shell *shell)
//{
////    xSemaphoreTakeRecursive(shellMutex, portMAX_DELAY);
//    return 0;
//}

/**
 * @brief 用户shell解锁
 * 
 * @param shell shell
 * 
 * @return int 0
 */
//int userShellUnlock(Shell *shell)
//{
//    xSemaphoreGiveRecursive(shellMutex);
//    return 0;
//}

void vTaskShell(void *pvParameters)
{
	while(1)
	{
		char c;
		if (lwrb_read(&shell_rb, &c, 1) == 1)
		{
			shellHandler(&shell, c);
		}
		vTaskDelay(1);
	}
}

/**
 * @brief 用户shell初始化
 * 
 */
void userShellInit(void)
{
    shell.write = userShellWrite;
    shell.read = userShellRead;
//    shell.lock = userShellLock;
//    shell.unlock = userShellUnlock;
    lwrb_init(&shell_rb, uart_buffer, sizeof(uart_buffer));
    shellInit(&shell, shellBuffer, 512);
    xTaskCreate(vTaskShell, "TaskShell", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
}

void shellHandlerRb(uint8_t c)
{
	lwrb_write(&shell_rb, &c, 1);
}

int func(int i, char ch, char *str)
{
//	EfErrCode err = ef_set_and_save_env("testenv", "test11111");
//	err = ef_set_and_save_env("password", "08240824");
	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), func, func, test);

int get_link_status(int argc, char *argv[])
{
//    bool link = false;
//    status_t status =  PHY_GetLinkStatus(ENET, BOARD_ENET0_PHY_ADDRESS, &link);
//    if (kStatus_Success != status)
//    {
//    	PRINTF("\r\nCannot PHY_GetLinkStatus PHY.\r\n");
//    }
//
//    PRINTF("link status:%d\r\n", link);
	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), get_link_status, get_link_status, get_link_status);

int enable_log(int argc, char *argv[])
{
	if (argc != 2)
	{
		elog_raw("invalid param, usage: enable_log [1|0]\r\n");
		return -1;
	}
	int enable = atoi(argv[1]);
	elog_set_output_enabled(enable);
	elog_raw("log enable:%d\r\n", elog_get_output_enabled());
	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), enable_log, enable_log, enable_log);

int enable_log_color(int argc, char *argv[])
{
	if (argc != 2)
	{
		elog_raw("invalid param, usage: enable_log_color [1|0]\r\n");
		return -1;
	}
	int enable = atoi(argv[1]);
	elog_set_text_color_enabled(enable);

	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), enable_log_color, enable_log_color, enable_log_color);

int set_log_level(int argc, char *argv[])
{
	if (argc != 2)
	{
		elog_raw("invalid param, usage: set_log_level [0|1|2|3|4|5]\r\n");
		return -1;
	}

	int level = atoi(argv[1]);
	elog_set_filter_lvl(level);

	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), set_log_level, set_log_level, set_log_level);

int set_log_tag(int argc, char *argv[])
{
	if (argc <= 1)
	{
		elog_raw("invalid param, usage: set_log_tag tag1 tag2 ...\r\n");
		return -1;
	}

	for (int i = 1; i < argc; ++i)
	{
		elog_set_filter_tag(argv[i]);
	}

	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), set_log_tag, set_log_tag, set_log_tag);

int set_log_tag_level(int argc, char *argv[])
{
	if (argc != 3)
	{
		elog_raw("invalid param, usage: set_log_tag_level tag level ...\r\n");
		return -1;
	}

	int level = atoi(argv[2]);
	elog_set_filter_tag_lvl(argv[1], level);

	return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), set_log_tag_level, set_log_tag_level, set_log_tag_level);

int get_rng(int argc, char *argv[])
{
	if (argc != 2)
	{
		elog_raw("invalid param, usage: get_rng [0|1]\r\n");
		return -1;
	}

	int hw = atoi(argv[1]);

	hal_rng_status_t status = HAL_RngInit();
	if (status != kStatus_HAL_RngSuccess)
	{
		log_e("HAL_RngInit failed:%x", status);
		return -1;
	}

	uint32_t random = 0;

	if (hw)
	{
		status =  HAL_RngHwGetData((void *)&random, sizeof(random));
	} else
	{
		status =  HAL_RngGetData((void *)&random, sizeof(random));
	}

	if (status != kStatus_HAL_RngSuccess)
	{
		log_e("HAL_RngHwGetData failed:%x", status);
		return -1;
	}

	log_i("random:%d", random);
	HAL_RngDeinit();

	return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), get_rng, get_rng, get_rng);

int show_task(int argc, char *argv[])
{
	static char pcWriteBuffer[512] = {0};
	vTaskList(pcWriteBuffer);
	elog_raw("%s", pcWriteBuffer);
	vTaskGetRunTimeStats(pcWriteBuffer);
	elog_raw("%s", pcWriteBuffer);
	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), show_task, show_task, show_task);


