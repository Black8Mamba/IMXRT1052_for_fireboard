/*
 * bsp_i2c_eeprom.c
 *
 *  Created on: 2025年1月5日
 *      Author: yongjie
 */

#include "fsl_debug_console.h"

#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "fsl_lpi2c.h"
#include "bsp_i2c_eeprom.h"
#include "perf_counter/perf_counter.h"

uint32_t I2C_EEPROM_Page_Write( uint8_t ClientAddr,
                                uint8_t WriteAddr,
                                uint8_t* pBuffer,
                                uint8_t NumByteToWrite)
{
  lpi2c_master_transfer_t masterXfer = {0};
  status_t reVal = kStatus_Fail;

  if(NumByteToWrite>EEPROM_PAGE_SIZE)
  {
    PRINTF("NumByteToWrite>EEPROM_PageSize\r\n");
    return 1;
  }


  masterXfer.slaveAddress = (ClientAddr>>1);
  masterXfer.direction = kLPI2C_Write;
  masterXfer.subaddress = WriteAddr;
  masterXfer.subaddressSize = EEPROM_INER_ADDRESS_SIZE;
  masterXfer.data = pBuffer;
  masterXfer.dataSize = NumByteToWrite;
  masterXfer.flags = kLPI2C_TransferDefaultFlag;

  reVal = LPI2C_MasterTransferBlocking(EEPROM_I2C_MASTER, &masterXfer);

  if (reVal != kStatus_Success)
  {
      return 1;
  }

  return 0;

}

void I2C_EEPROM_Buffer_Write( uint8_t ClientAddr,
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
    I2C_EEPROM_Page_Write(ClientAddr, WriteAddr, pBuffer, count);
//    I2C_EEPROM_WaitStandbyState(ClientAddr);
    delay_ms(1);
    WriteAddr += count;
    pBuffer += count;
  }


  if(NumOfPage== 0 )
  {
    I2C_EEPROM_Page_Write(ClientAddr, WriteAddr, pBuffer, NumOfSingle);
//    I2C_EEPROM_WaitStandbyState(ClientAddr);
    delay_ms(2);
  }
  else
  {
    while(NumOfPage--)
    {
      I2C_EEPROM_Page_Write(ClientAddr, WriteAddr, pBuffer, EEPROM_PAGE_SIZE);
//      I2C_EEPROM_WaitStandbyState(ClientAddr);
      delay_ms(2);
      WriteAddr +=  EEPROM_PAGE_SIZE;
      pBuffer += EEPROM_PAGE_SIZE;
    }

    if(NumOfSingle != 0)
    {
      I2C_EEPROM_Page_Write(ClientAddr, WriteAddr, pBuffer, NumOfSingle);
//      I2C_EEPROM_WaitStandbyState(ClientAddr);
      delay_ms(2);
    }
  }
}

uint32_t I2C_EEPROM_BufferRead( uint8_t ClientAddr,
                                uint8_t ReadAddr,
                                uint8_t* pBuffer,
                                uint16_t NumByteToRead)
{
  lpi2c_master_transfer_t masterXfer = {0};
  status_t reVal = kStatus_Fail;

  masterXfer.slaveAddress = (ClientAddr>>1);
  masterXfer.direction = kLPI2C_Read;
  masterXfer.subaddress = (uint32_t)ReadAddr;
  masterXfer.subaddressSize = EEPROM_INER_ADDRESS_SIZE;
  masterXfer.data = pBuffer;
  masterXfer.dataSize = NumByteToRead;
  masterXfer.flags = kLPI2C_TransferDefaultFlag;

  reVal = LPI2C_MasterTransferBlocking(EEPROM_I2C_MASTER, &masterXfer);

  if (reVal != kStatus_Success)
  {
      return 1;
  }

  return 0;
}

static  uint32_t I2C_Timeout_Callback(uint8_t errorCode)
{
  PRINTF("I2C wait timeout errorCode = %d",errorCode);

  return 0xFF;
}

uint8_t I2C_EEPROM_WaitStandbyState(uint8_t ClientAddr)
{
  status_t lpi2c_status;
  uint32_t delay_count = I2CT_LONG_TIMEOUT;

  do
  {
    LPI2C_MasterClearStatusFlags(EEPROM_I2C_MASTER, kLPI2C_MasterNackDetectFlag);

    lpi2c_status = LPI2C_MasterStart(EEPROM_I2C_MASTER, (ClientAddr>>1), kLPI2C_Write);

    delay_us(40);

  }while(EEPROM_I2C_MASTER->MSR & kLPI2C_MasterNackDetectFlag && delay_count-- );

  LPI2C_MasterClearStatusFlags(EEPROM_I2C_MASTER, kLPI2C_MasterNackDetectFlag);

  lpi2c_status = LPI2C_MasterStop(EEPROM_I2C_MASTER);
  delay_us(10);

  if(delay_count == 0 || lpi2c_status != kStatus_Success)
  {
    I2C_Timeout_Callback(3);
    return 1;
  }

  return 0;
}

#define EEPROM_TEST_NUM           256
#define EEPORM_TEST_START_ADDR    0

uint8_t EEPROM_Buffer_Write[256];
uint8_t EEPROM_Buffer_Read[256];

uint8_t EEPROM_Test(void)
{
  uint16_t i;

  for ( i=0; i<EEPROM_TEST_NUM; i++ )
  {
    EEPROM_Buffer_Write[i] = i;

    PRINTF("write 0x%02X ", EEPROM_Buffer_Write[i]);
    if((i+1)%10 == 0 || i == (EEPROM_TEST_NUM-1))
        PRINTF("\r\n");
   }

  I2C_EEPROM_Buffer_Write(EEPROM_WRITE_ADDRESS_8_BIT,
                             EEPORM_TEST_START_ADDR,
                             EEPROM_Buffer_Write,
                             EEPROM_TEST_NUM);


  I2C_EEPROM_BufferRead(EEPROM_READ_ADDRESS_8_BIT,
                           EEPORM_TEST_START_ADDR,
                           EEPROM_Buffer_Read,
                           EEPROM_TEST_NUM);


  for (i=0; i<EEPROM_TEST_NUM; i++)
  {
    if(EEPROM_Buffer_Read[i] != EEPROM_Buffer_Write[i])
    {
      PRINTF(" read failed 0x%02X ", EEPROM_Buffer_Read[i]);
      return 1;
    }
    PRINTF("read 0x%02X ", EEPROM_Buffer_Read[i]);
    if((i+1)%10 == 0 || i == (EEPROM_TEST_NUM-1))
        PRINTF("\r\n");

  }

  PRINTF("eeprom test pass!\n");
  return 0;
}


