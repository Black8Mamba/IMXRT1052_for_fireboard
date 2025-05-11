/*
 * bsp_norflash.c
 *
 *  Created on: 2025年3月15日
 *      Author: yongjie
 */

#include "fsl_flexspi.h"
#include "clock_config.h"
#include "fsl_common.h"
#include "bsp_norflash.h"

#define NOR_CMD_LUT_SEQ_IDX_READ_NORMAL                 0
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST                   1
#define NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD              2
#define NOR_CMD_LUT_SEQ_IDX_READSTATUS                  3
#define NOR_CMD_LUT_SEQ_IDX_WRITEENABLE                 4
#define NOR_CMD_LUT_SEQ_IDX_ERASESECTOR                 5
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE          6
#define NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD            7
#define NOR_CMD_LUT_SEQ_IDX_READID                      8
#define NOR_CMD_LUT_SEQ_IDX_READJEDECID                 9
#define NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG              10
#define NOR_CMD_LUT_SEQ_IDX_READSTATUSREG               11
#define NOR_CMD_LUT_SEQ_IDX_ERASECHIP                   12
#define NOR_CMD_LUT_SEQ_IDX_AHB_PAGEPROGRAM_QUAD_1      13
#define NOR_CMD_LUT_SEQ_IDX_AHB_PAGEPROGRAM_QUAD_2      14
#define NOR_CMD_LUT_SEQ_IDX_READ_UUID_ISSI              15
#define NOR_CMD_LUT_SEQ_IDX_READ_UUID_WB                16
#define NOR_CMD_LUT_SEQ_IDX_ERASEBLOCK 					17

#define CUSTOM_LUT_LENGTH           90

flexspi_device_config_t deviceconfig = {
    .flexspiRootClk = 120000000,
    .flashSize = FLASH_SIZE,
    .CSIntervalUnit = kFLEXSPI_CsIntervalUnit1SckCycle,
    .CSInterval = 2,
    .CSHoldTime = 1,
    .CSSetupTime = 1,
    .dataValidTime = 2,
    .columnspace = 0,
    .enableWordAddress = false,
    .AWRSeqIndex = NOR_CMD_LUT_SEQ_IDX_AHB_PAGEPROGRAM_QUAD_1,
    .AWRSeqNumber = 2,
    .ARDSeqIndex = NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD,
    .ARDSeqNumber = 1,
    .AHBWriteWaitUnit = kFLEXSPI_AhbWriteWaitUnit32768AhbCycle,
    /* W25Q256 typical time=0.7ms,max time=3ms
     *  fAHB = 528MHz,T AHB = 1/528us
     *  unit = 32768/600 = 54.61us
     *  AHBWriteWaitInterval = 1*1000/54.61 = 19
     */
    .AHBWriteWaitInterval = 19,
};

const uint32_t customLUT[CUSTOM_LUT_LENGTH] = {
        /* Normal read mode -SDR */
        [4 * NOR_CMD_LUT_SEQ_IDX_READ_NORMAL] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_ReadData_4Addr,
                            kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, FLASH_ADDR_LENGTH),

        [4 * NOR_CMD_LUT_SEQ_IDX_READ_NORMAL + 1] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04,
                            kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

        /* Fast read mode - SDR */
        [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_FastReadData_4Addr,
                            kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, FLASH_ADDR_LENGTH),

        [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST + 1] =
          FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 0x08,
                          kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

        /* Fast read quad mode - SDR */
        [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_FastReadQuad_4Addr,
                            kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_4PAD, FLASH_ADDR_LENGTH),
        [4 * NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD + 1] =
          FLEXSPI_LUT_SEQ(kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_4PAD, 0x06,
                          kFLEXSPI_Command_READ_SDR, kFLEXSPI_4PAD, 0x04),

        /* Read extend parameters */
        [4 * NOR_CMD_LUT_SEQ_IDX_READSTATUS] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_ReadStatusReg,
                            kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

        /* Write Enable */
        [4 * NOR_CMD_LUT_SEQ_IDX_WRITEENABLE] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_WriteEnable,
                            kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

        /* Erase Sector  */
        [4 * NOR_CMD_LUT_SEQ_IDX_ERASESECTOR] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_SectorErase_4Addr,
                            kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, FLASH_ADDR_LENGTH),

        /* Page Program - single mode */
        [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_PageProgram_4Addr,
                            kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, FLASH_ADDR_LENGTH),
        [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_SINGLE + 1] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04,
                            kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

        /* Page Program - quad mode */
        [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_PageProgramQuad_4Addr,
                            kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, FLASH_ADDR_LENGTH),
        [4 * NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD + 1] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04,
                            kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

        /* Read ID £¬ID7-ID0*/
        [4 * NOR_CMD_LUT_SEQ_IDX_READID] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_DeviceID,
                            kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, FLASH_ADDR_LENGTH),
        [4 * NOR_CMD_LUT_SEQ_IDX_READID + 1] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04,
                            kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

        /* JedecDeviceID,MF7-MF0+ID15-ID0 */
        [4 * NOR_CMD_LUT_SEQ_IDX_READJEDECID] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_JedecDeviceID,
                            kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

        /* Enable Quad mode */
        [4 * NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_WriteStatusReg,
                            kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_1PAD, 0x04),

        /* Read status register */
        [4 * NOR_CMD_LUT_SEQ_IDX_READSTATUSREG] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_ReadStatusReg,
                            kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04),

        /* Erase Chip */
        [4 * NOR_CMD_LUT_SEQ_IDX_ERASECHIP] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_ChipErase,
                            kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

        [4 * NOR_CMD_LUT_SEQ_IDX_AHB_PAGEPROGRAM_QUAD_1] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_WriteEnable,
                            kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

        /* Page Program - quad mode */
        [4 * NOR_CMD_LUT_SEQ_IDX_AHB_PAGEPROGRAM_QUAD_2] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_PageProgramQuad_4Addr,
                            kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, FLASH_ADDR_LENGTH),
        [4 * NOR_CMD_LUT_SEQ_IDX_AHB_PAGEPROGRAM_QUAD_2 + 1] =
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_WRITE_SDR, kFLEXSPI_4PAD, 0x04,
                            kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

        // read ID ISSID 0X4B/
        [4 * NOR_CMD_LUT_SEQ_IDX_READ_UUID_ISSI] 	=
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR,kFLEXSPI_1PAD,0x4B,
                              kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, FLASH_ADDR_LENGTH),

         [4 * NOR_CMD_LUT_SEQ_IDX_READ_UUID_ISSI + 1] 	=
             FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR,kFLEXSPI_1PAD,0x04,
                              kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),

        // read windbond id 0X4B/
        [4 * NOR_CMD_LUT_SEQ_IDX_READ_UUID_WB] 	=
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, 0x4B,
                              kFLEXSPI_Command_DUMMY_SDR, kFLEXSPI_1PAD, 0x18),

        [4 * NOR_CMD_LUT_SEQ_IDX_READ_UUID_WB + 1] 	=
            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_READ_SDR, kFLEXSPI_1PAD, 0x04,
                              kFLEXSPI_Command_STOP, kFLEXSPI_1PAD, 0),
		[4 * NOR_CMD_LUT_SEQ_IDX_ERASEBLOCK] =
	            FLEXSPI_LUT_SEQ(kFLEXSPI_Command_SDR, kFLEXSPI_1PAD, W25Q_BlockErase_4Addr,
	                            kFLEXSPI_Command_RADDR_SDR, kFLEXSPI_1PAD, FLASH_ADDR_LENGTH),
};

static void NorFlash_FlexSPI_ModeInit(void)
{
	  SCB_DisableDCache();
	  flexspi_config_t config;
	  FLEXSPI_GetDefaultConfig(&config);

	  config.ahbConfig.enableAHBPrefetch = true;

	  FLEXSPI_Init(FLEXSPI, &config);

	  FLEXSPI_SetFlashConfig(FLEXSPI, &deviceconfig, kFLEXSPI_PortA1);

	  FLEXSPI_UpdateLUT(FLEXSPI, 0, customLUT, CUSTOM_LUT_LENGTH);
}

status_t FlexSPI_NorFlash_Get_JedecDevice_ID(FLEXSPI_Type *base, uint32_t *vendorID)
{
    uint32_t temp;
    flexspi_transfer_t flashXfer;

    flashXfer.deviceAddress = 0;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Read;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_READJEDECID;
    flashXfer.data = &temp;
    flashXfer.dataSize = 3;

    status_t status = FLEXSPI_TransferBlocking(base, &flashXfer);

    *vendorID = ((temp&0xFF)<<16) | (temp&0xFF00) | ((temp&0xFF0000)>>16);

    return status;
}

status_t FlexSPI_NorFlash_Get_Device_ID(FLEXSPI_Type *base, uint8_t *vendorID)
{
    uint32_t temp;
    flexspi_transfer_t flashXfer;

    flashXfer.deviceAddress = 0;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Read;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_READID;
    flashXfer.data = &temp;
    flashXfer.dataSize = 1;

    status_t status = FLEXSPI_TransferBlocking(base, &flashXfer);

    *vendorID = temp;

    return status;
}

uint8_t FlexSPI_FlashUUID_Get_ISSI(uint8_t *buf)
{
    flexspi_transfer_t FlashTransfer;

    FlashTransfer.deviceAddress=0;
    FlashTransfer.port=kFLEXSPI_PortA1;
    FlashTransfer.cmdType=kFLEXSPI_Read;
    FlashTransfer.SeqNumber=1;
    FlashTransfer.seqIndex=NOR_CMD_LUT_SEQ_IDX_READ_UUID_ISSI;
    FlashTransfer.data=(uint32_t*)buf;
    FlashTransfer.dataSize=16;
    FLEXSPI_TransferBlocking(FLEXSPI,&FlashTransfer);
    return 1;
}

uint8_t FlexSPI_FlashUUID_Get_WB(uint8_t *buf)
{
    flexspi_transfer_t FlashTransfer;

    FlashTransfer.deviceAddress=0;
    FlashTransfer.port=kFLEXSPI_PortA1;
    FlashTransfer.cmdType=kFLEXSPI_Read;
    FlashTransfer.SeqNumber=1;
    FlashTransfer.seqIndex=NOR_CMD_LUT_SEQ_IDX_READ_UUID_WB;
    FlashTransfer.data=(uint32_t*)buf;
    FlashTransfer.dataSize=8;
    FLEXSPI_TransferBlocking(FLEXSPI,&FlashTransfer);
    return 1;

}

status_t FlexSPI_NorFlash_Write_Enable(FLEXSPI_Type *base)
{
    flexspi_transfer_t flashXfer;
    status_t status;

    flashXfer.deviceAddress = 0;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Command;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_WRITEENABLE;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

status_t FlexSPI_NorFlash_Wait_Bus_Busy(FLEXSPI_Type *base)
{
    bool isBusy;
    uint32_t readValue;
    status_t status;
    flexspi_transfer_t flashXfer;

    flashXfer.deviceAddress = 0;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Read;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_READSTATUSREG;
    flashXfer.data = &readValue;
    flashXfer.dataSize = 1;

    do
    {
        status = FLEXSPI_TransferBlocking(base, &flashXfer);

        if (status != kStatus_Success)
        {
            return status;
        }

        if (FLASH_BUSY_STATUS_POL)
        {

            if (readValue & (1U << FLASH_BUSY_STATUS_OFFSET))
            {
                isBusy = true;
            }
            else
            {
                isBusy = false;
            }
        }
        else
        {
            if (readValue & (1U << FLASH_BUSY_STATUS_OFFSET))
            {
                isBusy = false;
            }
            else
            {
                isBusy = true;
            }
        }

    } while (isBusy);

    return status;
}

status_t FlexSPI_NorFlash_Read_Status_Register(FLEXSPI_Type *base, uint8_t *SR_value)
{

    uint32_t readValue;
    status_t status;
    flexspi_transfer_t flashXfer;

    flashXfer.deviceAddress = 0;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Read;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_READSTATUSREG;
    flashXfer.data = &readValue;
    flashXfer.dataSize = 1;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    *SR_value = readValue;

    return status;
}

status_t FlexSPI_NorFlash_Write_Status_Register(FLEXSPI_Type *base, uint8_t *config)
{
	flexspi_transfer_t flashXfer;
    status_t status;

    status = FlexSPI_NorFlash_Write_Enable(base);

    if (status != kStatus_Success)
    {
        return status;
    }

    flashXfer.deviceAddress = 0;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Write;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_WRITESTATUSREG;
    flashXfer.data = (uint32_t *)config;
    flashXfer.dataSize = 1;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    if (status != kStatus_Success)
    {
        return status;
    }

    status = FlexSPI_NorFlash_Wait_Bus_Busy(base);

    return status;
}

status_t FlexSPI_NorFlash_Enable_Quad_Mode(FLEXSPI_Type *base)
{
	uint8_t SR_value;
	status_t status;

	status = FlexSPI_NorFlash_Read_Status_Register(base, &SR_value);

	if (status != kStatus_Success)
	{
		return status;
	}

	SR_value |= 0x40;

	status = FlexSPI_NorFlash_Write_Status_Register(base, &SR_value);

	return status;
}

status_t FlexSPI_NorFlash_Erase_Sector(FLEXSPI_Type *base, uint32_t dstAddr)
{
    status_t status;
    flexspi_transfer_t flashXfer;

    status = FlexSPI_NorFlash_Write_Enable(base);

    if (status != kStatus_Success)
    {
        return status;
    }

    flashXfer.deviceAddress = dstAddr;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Command;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_ERASESECTOR;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    if (status != kStatus_Success)
    {
        return status;
    }

    status = FlexSPI_NorFlash_Wait_Bus_Busy(base);

    return status;
}

status_t FlexSPI_NorFlash_Erase_Block(FLEXSPI_Type *base, uint32_t dstAddr)
{
    status_t status;
    flexspi_transfer_t flashXfer;

    status = FlexSPI_NorFlash_Write_Enable(base);

    if (status != kStatus_Success)
    {
        return status;
    }

    flashXfer.deviceAddress = dstAddr;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Command;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_ERASEBLOCK;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    if (status != kStatus_Success)
    {
        return status;
    }

    status = FlexSPI_NorFlash_Wait_Bus_Busy(base);

    return status;
}

status_t FlexSPI_NorFlash_Page_Program(FLEXSPI_Type *base,
                                       uint32_t dstAddr,
                                       uint8_t *src,
                                       uint16_t dataSize)
{
    status_t status;
    flexspi_transfer_t flashXfer;

    status = FlexSPI_NorFlash_Write_Enable(base);

    if (status != kStatus_Success)
    {
        return status;
    }

    flashXfer.deviceAddress = dstAddr;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Write;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_PAGEPROGRAM_QUAD;
    flashXfer.data = (uint32_t *)src;
    flashXfer.dataSize = dataSize;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    if (status != kStatus_Success)
    {
        return status;
    }

    status = FlexSPI_NorFlash_Wait_Bus_Busy(base);

    return status;
}

status_t FlexSPI_NorFlash_Buffer_Program(FLEXSPI_Type *base,
                                         uint32_t dstAddr,
                                         uint8_t *src,
                                         uint16_t dataSize)
{
  status_t status = kStatus_Success;
  uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
  uint16_t NumByteToWriteRest = dataSize;
  Addr = dstAddr % FLASH_PAGE_SIZE;
  count = FLASH_PAGE_SIZE - Addr;

  NumByteToWriteRest = (dataSize > count) ? (dataSize - count) : dataSize;

  NumOfPage =  NumByteToWriteRest / FLASH_PAGE_SIZE;
  NumOfSingle = NumByteToWriteRest % FLASH_PAGE_SIZE;

  if(count != 0 && dataSize > count)
  {
    status = FlexSPI_NorFlash_Page_Program(base, dstAddr, src, count);
    if(status != kStatus_Success) return status;

    dstAddr += count;
    src += count;
  }

  if(NumOfPage== 0 )
  {
    status = FlexSPI_NorFlash_Page_Program(base, dstAddr, src, NumOfSingle);
    if(status != kStatus_Success) return status;
  }
  else
  {
    while(NumOfPage--)
    {
      status = FlexSPI_NorFlash_Page_Program(base, dstAddr, src, FLASH_PAGE_SIZE);
      if(status != kStatus_Success) return status;

      dstAddr +=  FLASH_PAGE_SIZE;
      src += FLASH_PAGE_SIZE;
    }
    if(NumOfSingle != 0)
    {
      status = FlexSPI_NorFlash_Page_Program(base, dstAddr, src, NumOfSingle);
      if(status != kStatus_Success) return status;

    }
  }

  return status;
}

status_t FlexSPI_NorFlash_Buffer_Read(FLEXSPI_Type *base,
                                      uint32_t address,
                                      uint8_t *dst,
                                      uint16_t dataSize)
{
    status_t status;
    flexspi_transfer_t flashXfer;

    flashXfer.deviceAddress = address;
    flashXfer.port = kFLEXSPI_PortA1;
    flashXfer.cmdType = kFLEXSPI_Read;
    flashXfer.SeqNumber = 1;
    flashXfer.seqIndex = NOR_CMD_LUT_SEQ_IDX_READ_FAST_QUAD;
    flashXfer.data = (uint32_t *)dst;
    flashXfer.dataSize = dataSize;

    status = FLEXSPI_TransferBlocking(base, &flashXfer);

    return status;
}

int FlexSPI_NorFlash_Init(void)
{
	uint32_t JedecDeviceID;
	uint32_t WindBondDeviceID;
	uint8_t uuid[16];

	NorFlash_FlexSPI_ModeInit();
	FlexSPI_NorFlash_Get_JedecDevice_ID(FLEXSPI, &JedecDeviceID);
	FlexSPI_NorFlash_Get_Device_ID(FLEXSPI, &WindBondDeviceID);

	PRINTF("JedecDeviceID:%x\r\n", JedecDeviceID);
	PRINTF("WindBondDeviceID:%x\r\n", WindBondDeviceID);

	if (JedecDeviceID != FLASH_WINBOND_JEDECDEVICE_ID)
	{
		PRINTF("read FLASH_WINBOND_JEDECDEVICE_ID failed!\r\n");
		return -1;
	}

	if((JedecDeviceID &0xFF0000) == (FLASH_ISSI_JEDECDEVICE_ID &0xFF0000))
	{
		PRINTF("Enable Quad Mode!\n");
		FlexSPI_NorFlash_Enable_Quad_Mode(FLEXSPI);
	}

    FlexSPI_FlashUUID_Get_ISSI(uuid);
    PRINTF("FLASH UUID:");
    FLASH_DEBUG_ARRAY(uuid,16);

    return 0;
}

unsigned char buffer[4*1024] = {0};
void flash_test(void)
{
	status_t status = FlexSPI_NorFlash_Erase_Sector(FLEXSPI, 0x40000);
	if (status != kStatus_Success)
	{
		PRINTF("FlexSPI_NorFlash_Erase_Block failed!\r\n");
	}
	status = FlexSPI_NorFlash_Buffer_Read(FLEXSPI, 0x40000, buffer, 4*1024);
	if (status != kStatus_Success)
	{
		PRINTF("FlexSPI_NorFlash_Erase_Block failed!\r\n");
	}
	for (int i = 0; i < 256; ++i)
	{
		PRINTF("%02x ", buffer[i]);
	}
	PRINTF("\r\n");

    for (int i = 0; i < 64; ++i)
    {
  		status_t status = FlexSPI_NorFlash_Erase_Sector(FLEXSPI, 0x40000+i*0x1000);
//    	status_t status = FlexSPI_NorFlash_Erase_Sector(FLEXSPI, i*0x1000);
  		if (status != kStatus_Success)
  		{
  			PRINTF("FlexSPI_NorFlash_Erase_Block failed!\r\n");
  		}
    }

    PRINTF("erase2 success!\r\n");

//    for (int i = 0; i < 1024; ++i)
//    {
//    	unsigned char read_data[256] = {0};
//        status_t status = FlexSPI_NorFlash_Buffer_Read(FLEXSPI,
//                                               0+i*256,
//											   read_data,
//											   256);
//  		if (status != kStatus_Success)
//  		{
//  			PRINTF("FlexSPI_NorFlash_Page_Program failed!\r\n");
//  			return;
//  		}
////  		for (int j = 0; j < 256; ++j)
////  		{
////  			PRINTF("%02x ", read_data[j]);
////  		}
////  		PRINTF("\r\n");
//
//		  status = FlexSPI_NorFlash_Page_Program(FLEXSPI, 0x40000+i*256, read_data, 256);
//		  if (status != kStatus_Success)
//		  {
//	  			PRINTF("FlexSPI_NorFlash_Page_Program failed!\r\n");
//	  			return;
//		  }
//    }
//    PRINTF("read and write success!\r\n");
//	void pre_jump_app(void);
//    pre_jump_app();
//    jump_to_app();
}
