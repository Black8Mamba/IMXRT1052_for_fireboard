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
#include "easyflash.h"
#include "bsp_phy.h"
Shell shell;
char shellBuffer[512];

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
    shellInit(&shell, shellBuffer, 512);
//    if (xTaskCreate(shellTask, "shell", 256, &shell, 5, NULL) != pdPASS)
//    {
//        logError("shell task creat failed");
//    }
}


int func(int i, char ch, char *str)
{
	EfErrCode err = ef_set_and_save_env("testenv", "test11111");
	err = ef_set_and_save_env("password", "08240824");
	return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), func, func, test);

int get_link_status(int argc, char *argv[])
{
    bool link = false;
    status_t status =  PHY_GetLinkStatus(ENET, BOARD_ENET0_PHY_ADDRESS, &link);
    if (kStatus_Success != status)
    {
    	PRINTF("\r\nCannot PHY_GetLinkStatus PHY.\r\n");
    }

    PRINTF("link status:%d\r\n", link);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), get_link_status, get_link_status, get_link_status);
