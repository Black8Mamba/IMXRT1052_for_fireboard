/*
 * bsp_sd.c
 *
 *  Created on: 2025年3月6日
 *      Author: yongjie
 */


#include <stdio.h>
#include "fsl_debug_console.h"
#include "fsl_sd.h"
#include "sdmmc_config.h"
#include "fsl_common.h"
#include "bsp_sd.h"
#include "perf_counter/perf_counter.h"

/*! @brief Data block count accessed in card */
#define DATA_BLOCK_COUNT (5U)
/*! @brief Start data block number accessed in card */
#define DATA_BLOCK_START (2U)
/*! @brief Data buffer size. */
#define DATA_BUFFER_SIZE (FSL_SDMMC_DEFAULT_BLOCK_SIZE * DATA_BLOCK_COUNT)

sd_card_t g_sd;
SDK_ALIGN(uint8_t g_dataWrite[DATA_BUFFER_SIZE], BOARD_SDMMC_DATA_BUFFER_ALIGN_SIZE);
/*! @brief Data read from the card */
SDK_ALIGN(uint8_t g_dataRead[DATA_BUFFER_SIZE], BOARD_SDMMC_DATA_BUFFER_ALIGN_SIZE);

static void CardInformationLog(sd_card_t *card)
{
    assert(card);

    PRINTF("\r\nCard size %d * %d bytes\r\n", card->blockCount, card->blockSize);
    PRINTF("\r\nWorking condition:\r\n");
    if (card->operationVoltage == kSDMMC_OperationVoltage330V)
    {
        PRINTF("\r\n  Voltage : 3.3V\r\n");
    }
    else if (card->operationVoltage == kSDMMC_OperationVoltage180V)
    {
        PRINTF("\r\n  Voltage : 1.8V\r\n");
    }

    if (card->currentTiming == kSD_TimingSDR12DefaultMode)
    {
        if (card->operationVoltage == kSDMMC_OperationVoltage330V)
        {
            PRINTF("\r\n  Timing mode: Default mode\r\n");
        }
        else if (card->operationVoltage == kSDMMC_OperationVoltage180V)
        {
            PRINTF("\r\n  Timing mode: SDR12 mode\r\n");
        }
    }
    else if (card->currentTiming == kSD_TimingSDR25HighSpeedMode)
    {
        if (card->operationVoltage == kSDMMC_OperationVoltage180V)
        {
            PRINTF("\r\n  Timing mode: SDR25\r\n");
        }
        else
        {
            PRINTF("\r\n  Timing mode: High Speed\r\n");
        }
    }
    else if (card->currentTiming == kSD_TimingSDR50Mode)
    {
        PRINTF("\r\n  Timing mode: SDR50\r\n");
    }
    else if (card->currentTiming == kSD_TimingSDR104Mode)
    {
        PRINTF("\r\n  Timing mode: SDR104\r\n");
    }
    else if (card->currentTiming == kSD_TimingDDR50Mode)
    {
        PRINTF("\r\n  Timing mode: DDR50\r\n");
    }

    PRINTF("\r\n  Freq : %d HZ\r\n", card->busClock_Hz);
}

static status_t AccessCard(sd_card_t *card, bool isReadOnly)
{
    if (isReadOnly)
    {
        PRINTF("\r\nRead one data block......\r\n");
        if (kStatus_Success != SD_ReadBlocks(card, g_dataRead, DATA_BLOCK_START, 1U))
        {
            PRINTF("Read one data block failed.\r\n");
            return kStatus_Fail;
        }

        PRINTF("Read multiple data blocks......\r\n");
        if (kStatus_Success != SD_ReadBlocks(card, g_dataRead, DATA_BLOCK_START, DATA_BLOCK_COUNT))
        {
            PRINTF("Read multiple data blocks failed.\r\n");
            return kStatus_Fail;
        }
    }
    else
    {
    	//eraser sd card


        memset(g_dataWrite, 0x67U, sizeof(g_dataWrite));

        PRINTF("\r\nWrite/read one data block......\r\n");
        if (kStatus_Success != SD_WriteBlocks(card, g_dataWrite, DATA_BLOCK_START, 1U))
        {
            PRINTF("Write one data block failed.\r\n");
            return kStatus_Fail;
        }

        memset(g_dataRead, 0U, sizeof(g_dataRead));
        if (kStatus_Success != SD_ReadBlocks(card, g_dataRead, DATA_BLOCK_START, 1U))
        {
            PRINTF("Read one data block failed.\r\n");
            return kStatus_Fail;
        }

        PRINTF("Compare the read/write content......\r\n");
        if (memcmp(g_dataRead, g_dataWrite, FSL_SDMMC_DEFAULT_BLOCK_SIZE))
        {
            PRINTF("The read/write content isn't consistent.\r\n");
            return kStatus_Fail;
        }
        PRINTF("The read/write content is consistent.\r\n");

        PRINTF("Write/read multiple data blocks......\r\n");
        if (kStatus_Success != SD_WriteBlocks(card, g_dataWrite, DATA_BLOCK_START, DATA_BLOCK_COUNT))
        {
            PRINTF("Write multiple data blocks failed.\r\n");
            return kStatus_Fail;
        }

        memset(g_dataRead, 0U, sizeof(g_dataRead));

        if (kStatus_Success != SD_ReadBlocks(card, g_dataRead, DATA_BLOCK_START, DATA_BLOCK_COUNT))
        {
            PRINTF("Read multiple data blocks failed.\r\n");
            return kStatus_Fail;
        }

        PRINTF("Compare the read/write content......\r\n");
        if (memcmp(g_dataRead, g_dataWrite, FSL_SDMMC_DEFAULT_BLOCK_SIZE))
        {
            PRINTF("The read/write content isn't consistent.\r\n");
            return kStatus_Fail;
        }
        PRINTF("The read/write content is consistent.\r\n");

        PRINTF("Erase multiple data blocks......\r\n");
//        if (kStatus_Success != SD_EraseBlocks(card, DATA_BLOCK_START, DATA_BLOCK_COUNT))
        if (kStatus_Success != SD_EraseBlocks(card, DATA_BLOCK_START, 12473548))
        {
            PRINTF("Erase multiple data blocks failed.\r\n");
            return kStatus_Fail;
        }
    }

    return kStatus_Success;
}

int SDCard_Init(void)
{
	sd_card_t *card = &g_sd;

	 BOARD_SD_Config(card, NULL, BOARD_SDMMC_SD_HOST_IRQ_PRIORITY, NULL);
	 PRINTF("\r\nSDCARD polling example.\r\n");

	    /* SD host init function */
	 if (SD_HostInit(card) != kStatus_Success)
	 {
		 PRINTF("\r\nSD host init fail\r\n");
		 return -1;
	 }

     /* Init card. */
     if (SD_CardInit(card))
     {
         PRINTF("\r\nSD card init failed.\r\n");
         return -1;
     }

	 PRINTF("\r\nSDCARD host and card init success.\r\n");
}

void SDCard_Test(void)
{
	sd_card_t *card = &g_sd;
    /* card information log */
    CardInformationLog(card);

    /* Check if card is readonly. */
	 bool isReadOnly = SD_CheckReadOnly(card);

    PRINTF("\r\nRead/Write/Erase the card continuously until encounter error......\r\n");

    PRINTF("isReadOnly:%d\n", isReadOnly);

    if (kStatus_Success != AccessCard(card, isReadOnly))
    {
        /* access card fail, due to card remove. */
        if (SD_IsCardPresent(card) == false)
        {
            SD_HostDoReset(card);
            PRINTF("\r\nCard removed\r\n");
            PRINTF(
                "\r\nInput 'q' to quit read/write/erase process.\
        \r\nInput other char to wait card re-insert.\r\n");
        }
        /* access card fail, due to transfer error */
        else
        {
        }
    }
    else
    {
        PRINTF(
            "\r\nInput 'q' to quit read/write/erase process.\
        \r\nInput other char to read/write/erase data blocks again.\r\n");
    }
}
