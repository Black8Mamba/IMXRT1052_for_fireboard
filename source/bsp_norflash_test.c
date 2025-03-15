/*
 * bsp_norflash_test.c
 *
 *  Created on: 2025年3月15日
 *      Author: yongjie
 */


#include "fsl_flexspi.h"
#include "bsp_norflash.h"

#define EXAMPLE_SECTOR      4096
#define EXAMPLE_SIZE        (4*1024)

static uint8_t s_nor_program_buffer[EXAMPLE_SIZE];
static uint8_t s_nor_read_buffer[EXAMPLE_SIZE];

extern status_t FlexSPI_NorFlash_Enable_QE(FLEXSPI_Type *base);
extern uint8_t FlexSPI_FlashUUID_Get_ISSI(uint8_t *buf);

int NorFlash_IPCommand_Test(void)
{
    uint32_t i = 0;
    status_t status;
    uint8_t uuid[16];
    uint32_t JedecDeviceID = 0;

    PRINTF("\r\nNorFlash IP test begin\r\n");

    FlexSPI_NorFlash_Get_JedecDevice_ID(FLEXSPI, &JedecDeviceID);

    if(JedecDeviceID != FLASH_WINBOND_JEDECDEVICE_ID &&
			 JedecDeviceID != FLASH_ISSI_JEDECDEVICE_ID)
    {
      PRINTF("FLASH detect Jedec ID error: 0x%x\r\n", JedecDeviceID);
      return -1;
    }

    PRINTF("Jedec:%x\r\n", JedecDeviceID);

    FlexSPI_FlashUUID_Get_ISSI(uuid);
    PRINTF("FLASH UUID:");
    FLASH_DEBUG_ARRAY(uuid,16);

    PRINTF("erase sector test begin\r\n");

    status = FlexSPI_NorFlash_Erase_Sector(FLEXSPI, EXAMPLE_SECTOR * SECTOR_SIZE);
    if (status != kStatus_Success)
    {
        PRINTF("FlexSPI_NorFlash_Erase_Sector failed!\r\n");
        return -1;
    }

    status = FlexSPI_NorFlash_Buffer_Read(FLEXSPI,
                                         EXAMPLE_SECTOR * SECTOR_SIZE,
                                         s_nor_read_buffer,
                                         EXAMPLE_SIZE);

    if (status != kStatus_Success)
    {
        PRINTF("FlexSPI_NorFlash_Buffer_Read failed\r\n");
        return -1;
    }

    memset(s_nor_program_buffer, 0xFF, EXAMPLE_SIZE);
    if (memcmp(s_nor_program_buffer, s_nor_read_buffer, EXAMPLE_SIZE))
    {
        PRINTF("read data compare error after erase,!=0xff!\r\n ");
        return -1;
    }
    else
    {
        PRINTF("erase test success\r\n");
    }

    PRINTF("write test begin\r\n");

    for (i = 0; i < EXAMPLE_SIZE; i++)
    {
        s_nor_program_buffer[i] = (uint8_t)i;
    }

    status = FlexSPI_NorFlash_Buffer_Program(FLEXSPI,
                                             EXAMPLE_SECTOR * SECTOR_SIZE,
                                             (void *)s_nor_program_buffer,
                                             EXAMPLE_SIZE);

    if (status != kStatus_Success)
    {
        PRINTF("FlexSPI_NorFlash_Buffer_Program failed!\r\n");
        return -1;
    }

    status = FlexSPI_NorFlash_Buffer_Read(FLEXSPI,
                                           EXAMPLE_SECTOR * SECTOR_SIZE,
                                           s_nor_read_buffer,
                                           EXAMPLE_SIZE);
    if (status != kStatus_Success)
    {
        PRINTF("FlexSPI_NorFlash_Buffer_Read failed!\r\n");
        return -1;
    }

    if (memcmp(s_nor_program_buffer, s_nor_read_buffer, EXAMPLE_SIZE))
    {
        PRINTF("read data compare error after write!\r\n ");
        return -1;
    }
    else
    {
        PRINTF("write test success\r\n");
    }

    PRINTF("NorFlash IP test success\r\n");
    return 0;
}

int NorFlash_AHBCommand_Test(void)
{
    uint32_t i = 0;
    status_t status;
    uint8_t uuid[16];
    uint32_t JedecDeviceID = 0;

    PRINTF("\r\nNorFlash AHB test begin\r\n");

    FlexSPI_NorFlash_Get_JedecDevice_ID(FLEXSPI, &JedecDeviceID);

    if(JedecDeviceID != FLASH_WINBOND_JEDECDEVICE_ID &&
			 JedecDeviceID != FLASH_ISSI_JEDECDEVICE_ID)
    {
      PRINTF("FLASH detect Jedec ID error: 0x%x\r\n", JedecDeviceID);
      return -1;
    }

    PRINTF("Jedec:%x\r\n", JedecDeviceID);

    FlexSPI_FlashUUID_Get_ISSI(uuid);
    PRINTF("FLASH UUID:");
    FLASH_DEBUG_ARRAY(uuid,16);

    PRINTF("erase sector test begin\r\n");

    status = FlexSPI_NorFlash_Erase_Sector(FLEXSPI, EXAMPLE_SECTOR * SECTOR_SIZE);
    if (status != kStatus_Success)
    {
        PRINTF("FlexSPI_NorFlash_Erase_Sector failed!\r\n");
        return -1;
    }

    memcpy(s_nor_read_buffer,
           NORFLASH_AHB_POINTER(EXAMPLE_SECTOR * SECTOR_SIZE),
           EXAMPLE_SIZE);

    memset(s_nor_program_buffer, 0xFF, EXAMPLE_SIZE);
    if (memcmp(s_nor_program_buffer, s_nor_read_buffer, EXAMPLE_SIZE))
    {
        PRINTF("read data compare error after erase,!=0xff!\r\n ");
        return -1;
    }
    else
    {
        PRINTF("erase test success\r\n");
    }

    PRINTF("8 16 32bytes write test, 0x12, 0x3456, 0x789abcde\r\n");


    *(uint8_t *)NORFLASH_AHB_POINTER(EXAMPLE_SECTOR * SECTOR_SIZE) = 0x12;
    *(uint16_t *)NORFLASH_AHB_POINTER(EXAMPLE_SECTOR * SECTOR_SIZE + 4) = 0x3456;
    *(uint32_t *)NORFLASH_AHB_POINTER(EXAMPLE_SECTOR * SECTOR_SIZE + 8) = 0x789abcde;

    PRINTF("8bytes read = 0x%x\r\n",
           *(uint8_t *)NORFLASH_AHB_POINTER(EXAMPLE_SECTOR * SECTOR_SIZE));
    PRINTF("16bytes read = 0x%x\r\n",
           *(uint16_t *)NORFLASH_AHB_POINTER(EXAMPLE_SECTOR * SECTOR_SIZE+4));
    PRINTF("32bytes read = 0x%x\r\n",
           *(uint32_t *)NORFLASH_AHB_POINTER(EXAMPLE_SECTOR * SECTOR_SIZE+8));

    for (i = 0; i < EXAMPLE_SIZE; i++)
    {
        s_nor_program_buffer[i] = (uint8_t)i;
    }

    status = FlexSPI_NorFlash_Erase_Sector(FLEXSPI, EXAMPLE_SECTOR * SECTOR_SIZE);
    if (status != kStatus_Success)
    {
        PRINTF("FlexSPI_NorFlash_Erase_Sector!\r\n");
        return -1;
    }

    status = FlexSPI_NorFlash_Buffer_Program(FLEXSPI,
                                             EXAMPLE_SECTOR * SECTOR_SIZE,
                                             (void *)s_nor_program_buffer,
                                             EXAMPLE_SIZE);
    if (status != kStatus_Success)
    {
        PRINTF("FlexSPI_NorFlash_Buffer_Program!\r\n");
        return -1;
    }

    FLEXSPI_SoftwareReset(FLEXSPI);

    memcpy(s_nor_read_buffer,
           NORFLASH_AHB_POINTER(EXAMPLE_SECTOR * SECTOR_SIZE),
           EXAMPLE_SIZE);

    if (memcmp(s_nor_program_buffer, s_nor_read_buffer, EXAMPLE_SIZE))
    {
        PRINTF("read compare error after write!\r\n ");
        return -1;
    }
    else
    {
    	PRINTF("read compare success after write!\r\n ");
    }

    PRINTF("\r\nNorFlash AHB test end\r\n");
	return 0;
}
