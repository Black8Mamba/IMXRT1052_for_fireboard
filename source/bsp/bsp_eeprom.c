/*
 * bsp_eeprom.c
 *
 *  Created on: 2025年3月31日
 *      Author: yongjie
 */

#include <stdbool.h>
#include <stdint.h>
#include "elog.h"
#include "bsp_eeprom.h"
#include "perf_counter.h"

#undef LOG_TAG
#undef LOG_LVL
#define LOG_TAG    "eeprom"
#define LOG_LVL    ELOG_LVL_DEBUG

HAL_I2C_MASTER_HANDLE_DEFINE(i2cMasterHandle);




void i2c_eeprom_init(void)
{
	hal_i2c_master_config_t masterConfig;
	masterConfig.enableMaster   = true;
	masterConfig.baudRate_Bps   = 100000U;
	masterConfig.srcClock_Hz    = 12000000U;
	masterConfig.instance       = 1;
	hal_i2c_status_t status = HAL_I2cMasterInit((hal_i2c_master_handle_t)i2cMasterHandle, &masterConfig);
	if (status != kStatus_HAL_I2cSuccess)
	{
		log_e("HAL_I2cMasterInit failed:%x\n", status);
		return;
	}
}

int32_t i2c_eeprom_page_write( uint8_t ClientAddr,
                                uint8_t WriteAddr,
                                uint8_t* pBuffer,
                                uint8_t NumByteToWrite)
{
	hal_i2c_status_t status;

  if(NumByteToWrite>EEPROM_PAGE_SIZE)
  {
    log_e("NumByteToWrite>EEPROM_PageSize\r\n");
    return -1;
  }

  hal_i2c_master_transfer_t xfer;
  xfer.slaveAddress 	= (ClientAddr>>1);
  xfer.direction 		= kLPI2C_Write;
  xfer.subaddress 		= WriteAddr;
  xfer.subaddressSize 	= EEPROM_INER_ADDRESS_SIZE;
  xfer.data 			= pBuffer;
  xfer.dataSize 		= NumByteToWrite;
  xfer.flags 			= kLPI2C_TransferDefaultFlag;

  status = HAL_I2cMasterTransferBlocking((hal_i2c_master_handle_t)i2cMasterHandle, &xfer);
  if (status != kStatus_HAL_I2cSuccess)
  {
	  log_e("HAL_I2cMasterTransferBlocking failed:%d\n!", status);
	  return -2;
  }

  return 0;
}

void i2c_eeprom_buffer_write( uint8_t ClientAddr,
                              uint8_t WriteAddr,
                              uint8_t* pBuffer,
                              uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;

  uint8_t NumByteToWriteRest = NumByteToWrite;

  Addr = WriteAddr % EEPROM_PAGE_SIZE;
  count = EEPROM_PAGE_SIZE - Addr;


  NumByteToWriteRest = (NumByteToWrite > count) ? (NumByteToWrite - count) : NumByteToWrite;


  NumOfPage =  NumByteToWriteRest / EEPROM_PAGE_SIZE;

  NumOfSingle = NumByteToWriteRest % EEPROM_PAGE_SIZE;


  if(count != 0 && NumByteToWrite > count)
  {
	  i2c_eeprom_page_write(ClientAddr, WriteAddr, pBuffer, count);
	  delay_ms(5);
	  WriteAddr += count;
	  pBuffer += count;
  }


  if(NumOfPage== 0 )
  {
	  i2c_eeprom_page_write(ClientAddr, WriteAddr, pBuffer, NumOfSingle);
	  delay_ms(5);
  } else
  {
    while(NumOfPage--)
    {
    	i2c_eeprom_page_write(ClientAddr, WriteAddr, pBuffer, EEPROM_PAGE_SIZE);
    	delay_ms(5);
    	WriteAddr +=  EEPROM_PAGE_SIZE;
    	pBuffer += EEPROM_PAGE_SIZE;
    }

    if(NumOfSingle != 0)
    {
    	i2c_eeprom_page_write(ClientAddr, WriteAddr, pBuffer, NumOfSingle);
    	delay_ms(5);
    }
  }
}

int32_t i2c_eeprom_buffer_read( uint8_t ClientAddr,
                                uint8_t ReadAddr,
                                uint8_t* pBuffer,
                                uint16_t NumByteToRead)
{
	hal_i2c_status_t status;

	hal_i2c_master_transfer_t xfer;

	xfer.slaveAddress 	= (ClientAddr>>1);
	xfer.direction 		= kLPI2C_Read;
	xfer.subaddress 	= (uint32_t)ReadAddr;
	xfer.subaddressSize = EEPROM_INER_ADDRESS_SIZE;
	xfer.data 			= pBuffer;
	xfer.dataSize 		= NumByteToRead;
	xfer.flags 			= kLPI2C_TransferDefaultFlag;

	status = HAL_I2cMasterTransferBlocking((hal_i2c_master_handle_t)i2cMasterHandle, &xfer);
	if (status != kStatus_HAL_I2cSuccess)
	{
		log_e("HAL_I2cMasterTransferBlocking failed:%d\n!", status);
		return -2;
	}

  return 0;
}

#define EEPROM_TEST_NUM           256
#define EEPORM_TEST_START_ADDR    0

uint8_t EEPROM_Buffer_Write[256];
uint8_t EEPROM_Buffer_Read[256];

uint8_t eeprom_block_test(void)
{
  uint16_t i;

  I2C_EEPROM_Init();

  for (i = 0; i < EEPROM_TEST_NUM; i++ )
  {
	  EEPROM_Buffer_Write[i] = i;
  }

  i2c_eeprom_buffer_write(EEPROM_WRITE_ADDRESS_8_BIT,
                             EEPORM_TEST_START_ADDR,
                             EEPROM_Buffer_Write,
                             EEPROM_TEST_NUM);

  delay_ms(10);
  i2c_eeprom_buffer_read(EEPROM_READ_ADDRESS_8_BIT,
                           EEPORM_TEST_START_ADDR,
                           EEPROM_Buffer_Read,
                           EEPROM_TEST_NUM);

  for (i = 0; i < EEPROM_TEST_NUM; i++)
  {
    if(EEPROM_Buffer_Read[i] != EEPROM_Buffer_Write[i])
    {
    	log_i(" read failed 0x%02X ", EEPROM_Buffer_Read[i]);
      return 1;
    }
    log_i("read 0x%02X ", EEPROM_Buffer_Read[i]);
    if((i+1)%10 == 0 || i == (EEPROM_TEST_NUM-1))
    	log_i("\r\n");

  }

 log_i("eeprom test pass!\n");
  return 0;
}
