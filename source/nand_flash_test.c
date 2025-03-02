/**
  ******************************************************************
  * @file    nand_flash_test.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   NAND²âÊÔ³ÌÐò
  ******************************************************************
  * @attention
  *
  * ÊµÑéÆ½Ì¨:Ò°»ð  i.MXRT1052¿ª·¢°å
  * ÂÛÌ³    :http://www.firebbs.cn
  * ÌÔ±¦    :https://fire-stm32.taobao.com
  *
  ******************************************************************
  */

#include "bsp_nand.h"
#include "fsl_debug_console.h"
#include "fsl_nand_flash.h"
#include "fsl_semc_nand_flash.h"

uint8_t mem_writeBuffer[FLASH_PAGE_SIZE];
uint8_t mem_readBuffer[FLASH_PAGE_SIZE] = {0};
static nand_handle_t nandHandle;

extern nand_config_t nandConfig;
extern uint32_t NAND_ReadID(void);

static const char s_nandDeviceManufacturerList[][12] = {{'M', 'I', 'C', 'R', 'O', 'N', ' ', ' ', ' ', ' ', ' ', ' '},
                                                        {'S', 'P', 'A', 'N', 'S', 'I', 'O', 'N', ' ', ' ', ' ', ' '},
                                                        {'S', 'A', 'M', 'S', 'U', 'N', 'G', ' ', ' ', ' ', ' ', ' '},
                                                        {'W', 'I', 'N', 'B', 'O', 'N', 'D', ' ', ' ', ' ', ' ', ' '},
                                                        {'H', 'Y', 'N', 'I', 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                                        {'T', 'O', 'S', 'H', 'I', 'B', 'A', ' ', ' ', ' ', ' ', ' '},
                                                        {'M', 'A', 'C', 'R', 'O', 'N', 'I', 'X', ' ', ' ', ' ', ' '},
                                                        {0}};

void ErrorHandler(void)
{
    PRINTF("read write error!\n");
    while (1)
    {
    }
}
/**
* @brief  nand flash ¶ÁÐ´²âÊÔ³ÌÐò
* @param  ÎÞ
* @retval kStatus_Success£º³É¹¦
*/

/**
* @brief  读取NAND FLASH的ID
* @param  无
* @retval NAND FLASH的ID值
*/
uint32_t NAND_ReadID(void)
{
	uint16_t commandCode;

	status_t result = kStatus_Success;
	uint8_t rdata[5];
	uint32_t id;
	commandCode = SEMC_BuildNandIPCommand(0X90, kSEMC_NANDAM_ColumnRow, kSEMC_NANDCM_Command);

	result = SEMC_SendIPCommand(SEMC, kSEMC_MemType_NAND, 0, commandCode, 0, NULL);

	result |= SEMC_IPCommandNandRead(SEMC, 0, rdata, 5);

	if(result != kStatus_Success)
	{
		return 0;
	}
	id=((uint32_t)rdata[0])<<24|((uint32_t)rdata[1])<<16|((uint32_t)rdata[2])<<8|rdata[3];
	return id;
}

void nand_flash_init(void)
{
	status_t status;
    PRINTF("NAND Flash init!\n");
    status = Nand_Flash_Init(&nandConfig, &nandHandle);
    if (status != kStatus_Success)
    {
        PRINTF("NAND Flash init failed\n");
        PRINTF("error code: %d \n", status);
        ErrorHandler();
    }
    PRINTF("NAND Flash init success!\n");
}

void nand_flash_test(void)
{
	status_t status;
    char Manufacturer[12];

    nand_flash_init();

    PRINTF("NAND FlashID:0x%x\n", NAND_ReadID());
    memset(Manufacturer, 0, 12);
    strncpy(Manufacturer, (const char *)s_nandDeviceManufacturerList[nandHandle.vendorType], 8);
    PRINTF("\r\nNAND Flash vendor %s\n", Manufacturer);
    PRINTF("\r\nNAND Flash page size %d bytes\n", nandHandle.bytesInPageDataArea);
    PRINTF("\r\nNAND Flash page bak size %d bytes\r\n", nandHandle.bytesInPageSpareArea);
    PRINTF("\r\nNAND Flash block %d page\n", nandHandle.pagesInBlock);
    PRINTF("\r\nNAND Flash plane %d block\n", nandHandle.blocksInPlane);
    PRINTF("\r\nNAND Flash device %d plane\n", nandHandle.planesInDevice);

    //		PRINTF("\r\n***NAND Flash ID£º%d\r\n",nandHandle.JEDECid);
    //    SEMC->IPCR1 = 1;

#if 1

    /* Erase Block */
    PRINTF("\r\nNAND Flash Erase first block\n");
    status = Nand_Flash_Erase_Block(&nandHandle, 0);
    if (status != kStatus_Success)
    {
        PRINTF("\r\nNAND Flash erase block failed\n");
        ErrorHandler();
    }

    /* Read and check if it is blank. */
    PRINTF("\r\nNAND Flash erase check\n");
    for (uint32_t pageIndex = 0; pageIndex < nandHandle.pagesInBlock; pageIndex++)
    {
        status = Nand_Flash_Read_Page(&nandHandle, pageIndex, mem_readBuffer, nandHandle.bytesInPageDataArea);
        if (status != kStatus_Success)
        {
            PRINTF("\r\nNAND Flash read page failed\n");
            ErrorHandler();
        }

        for (uint32_t bytesIndex = 0; bytesIndex < nandHandle.bytesInPageDataArea; bytesIndex++)
        {
            if (mem_readBuffer[bytesIndex] != 0xFF)
            {
                PRINTF("\r\nNAND Flash erase page failed\n");
                ErrorHandler();
            }
        }
    }

    PRINTF("\r\nNAND Flash erase block success\n");

    /* Program the page data. */
    PRINTF("\r\nNAND Flash page write begin\n");
    /* Initialize the write buffers. */
    memset(mem_writeBuffer, 0xaa, sizeof(mem_writeBuffer));
    status = Nand_Flash_Page_Program(&nandHandle, 0, mem_writeBuffer, nandHandle.bytesInPageDataArea);
    if (status != kStatus_Success)
    {
        PRINTF("\r\nNAND Flash page write failed\n");
        ErrorHandler();
    }

    /* Read page data and check if the data read is equal to the data programed. */
    PRINTF("\r\nNAND Flash page read begin\n");
    status = Nand_Flash_Read_Page(&nandHandle, 0, mem_readBuffer, nandHandle.bytesInPageDataArea);
    if (status != kStatus_Success)
    {
        PRINTF("\r\nNAND Flash page read failed\n");
        ErrorHandler();
    }

    if (memcmp(mem_writeBuffer, mem_readBuffer, nandHandle.bytesInPageDataArea) != 0)
    {
        PRINTF("\r\nNAND Flash page read check failed\n");
        ErrorHandler();
    }
#endif
    PRINTF("\r\nNAND Flash read success\n");
}














