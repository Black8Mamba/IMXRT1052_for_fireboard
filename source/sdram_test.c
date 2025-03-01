/*
 * sdram_test.c
 *
 *  Created on: 2025年2月26日
 *      Author: yongjie
 */

#include "fsl_debug_console.h"

#define SDRAM_START_ADDRESS       (0x80000000U)
#define SDRAM_MEM_LENGTH          (32*1024*1024)

#define SEMC_EXAMPLE_DATALEN      (0x1000U)
#define SEMC_EXAMPLE_WRITETIMES   (1000U)


uint32_t sdram_writeBuffer[SEMC_EXAMPLE_DATALEN];
uint32_t sdram_readBuffer[SEMC_EXAMPLE_DATALEN];

bool SEMC_SDRAMReadWrite32Bit(void)
{
    uint32_t index;
    uint32_t datalen = SEMC_EXAMPLE_DATALEN;

    uint32_t *sdram = (uint32_t *)SDRAM_START_ADDRESS;
    bool result = true;

    PRINTF("\r\n write 32bit stard addr:0x%x, len:%d \r\n",sdram, datalen);


    for (index = 0; index < datalen; index++)
    {
        sdram_writeBuffer[index] = index;
        sdram[index] = sdram_writeBuffer[index];
    }

    PRINTF("\r\n read 32bit start addr:0x%d, len:%d \r\n",sdram, datalen);


    for (index = 0; index < datalen; index++)
    {
        sdram_readBuffer[index] = sdram[index];
    }

    PRINTF("\r\n ¿ªÊ¼±È½Ï¶ÁÐ´µÄÄÚÈÝ...\r\n");

    while (datalen--)
    {
        if (sdram_writeBuffer[datalen] != sdram_readBuffer[datalen])
        {
            result = false;
            break;
        }
    }

    if (!result)
    {
        PRINTF("\r\n 32bit read write compare failed\r\n");
    }
    else
    {
    	PRINTF("\r\n 32bit read write compare success\r\n");
    }

    return result;
}

static bool SEMC_SDRAMReadWrite16Bit(void)
{
    uint32_t index;
    uint32_t datalen = SEMC_EXAMPLE_DATALEN;
    uint16_t *sdram = (uint16_t *)SDRAM_START_ADDRESS;
    bool result = true;

    PRINTF("\r\n write 16bit stard addr:0x%x, len:%d \r\n",sdram, datalen);

    memset(sdram_writeBuffer, 0, sizeof(sdram_writeBuffer));
    memset(sdram_readBuffer, 0, sizeof(sdram_readBuffer));

    /* ÏòSDRAMÐ´ÈëÊý¾Ý */
    for (index = 0; index < datalen; index++)
    {
        sdram_writeBuffer[index] = index % 0xFFFF;
        sdram[index] = sdram_writeBuffer[index];
    }

    PRINTF("\r\n read 16bit start addr:0x%d, len:%d \r\n",sdram, datalen);


    for (index = 0; index < datalen; index++)
    {
        sdram_readBuffer[index] = sdram[index];
    }

    while (datalen--)
    {
        if (sdram_writeBuffer[datalen] != sdram_readBuffer[datalen])
        {
            result = false;
            break;
        }
    }

    if (!result)
    {
        PRINTF("\r\n 16bit read write compare failed\r\n");
    }
    else
    {
    	PRINTF("\r\n 16bit read write compare success\r\n");
    }

    return result;
}

static bool SEMC_SDRAMReadWrite8Bit(void)
{
    uint32_t index;
    uint32_t datalen = SEMC_EXAMPLE_DATALEN;
    uint8_t *sdram = (uint8_t *)SDRAM_START_ADDRESS;
    bool result = true;

    PRINTF("\r\n write 8bit stard addr:0x%x, len:%d \r\n",sdram, datalen);

    memset(sdram_writeBuffer, 0, sizeof(sdram_writeBuffer));
    memset(sdram_readBuffer, 0, sizeof(sdram_readBuffer));

    for (index = 0; index < datalen; index++)
    {
        sdram_writeBuffer[index] = index % 0x100;
        sdram[index] = sdram_writeBuffer[index];
    }

    PRINTF("\r\n read 8bit stard addr:0x%x, len:%d \r\n",sdram, datalen);

    for (index = 0; index < datalen; index++)
    {
        sdram_readBuffer[index] = sdram[index];
    }

    while (datalen--)
    {
        if (sdram_writeBuffer[datalen] != sdram_readBuffer[datalen])
        {
            result = false;
            break;
        }
    }

    if (!result)
    {
        PRINTF("\r\n 8bit read write compare failed\r\n");
    }
    else
    {
    	PRINTF("\r\n 8bit read write compare success\r\n");
    }

    return result;
}

bool SEMC_SDRAMReadWriteTest(void)
{
  bool result;

	result = SEMC_SDRAMReadWrite32Bit();
  if(!result) return result;

	result = SEMC_SDRAMReadWrite16Bit();
  if(!result) return result;

	result = SEMC_SDRAMReadWrite8Bit();

  return result;
}

bool SDRAM_FullChipTest(void)
{
  uint32_t counter=0;

  uint8_t ubWritedata_8b = 0, ubReaddata_8b = 0;

  uint16_t uhWritedata_16b = 0, uhReaddata_16b = 0;

  PRINTF("full test!\n");


  for (counter = 0x00; counter < SDRAM_MEM_LENGTH; counter++)
  {
    *(__IO uint8_t*) (SDRAM_START_ADDRESS + counter) = (uint8_t)0x0;
  }

  for (counter = 0; counter < SDRAM_MEM_LENGTH; counter++)
  {
    *(__IO uint8_t*) (SDRAM_START_ADDRESS + counter) = (uint8_t)(ubWritedata_8b + counter);
  }

  for(counter = 0; counter<SDRAM_MEM_LENGTH;counter++ )
  {

    ubReaddata_8b = *(__IO uint8_t*)(SDRAM_START_ADDRESS + counter);

    if(ubReaddata_8b != (uint8_t)(ubWritedata_8b + counter))
    {
      PRINTF("\r\n 8bit read write error\r\n");
      return false;
    }
  }

  for (counter = 0x00; counter < SDRAM_MEM_LENGTH/2; counter++)
  {
    *(__IO uint16_t*) (SDRAM_START_ADDRESS + 2*counter) = (uint16_t)0x00;
  }

  for (counter = 0; counter < SDRAM_MEM_LENGTH/2; counter++)
  {
    *(__IO uint16_t*) (SDRAM_START_ADDRESS + 2*counter) = (uint16_t)(uhWritedata_16b + counter);
  }

  for(counter = 0; counter<SDRAM_MEM_LENGTH/2;counter++ )
  {
    uhReaddata_16b = *(__IO uint16_t*)(SDRAM_START_ADDRESS + 2*counter);

    if(uhReaddata_16b != (uint16_t)(uhWritedata_16b + counter))
    {
      PRINTF("\r\n 16bit read write error\r\n");
      return false;
    }
  }

  return true;
}

/****************************END OF FILE**********************/
