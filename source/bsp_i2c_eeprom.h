/*
 * bsp_i2c_eeprom.h
 *
 *  Created on: 2025年1月5日
 *      Author: yongjie
 */

#ifndef BSP_I2C_EEPROM_H_
#define BSP_I2C_EEPROM_H_

#define EEPROM_I2C_MASTER_BASE   (LPI2C1_BASE)
#define EEPROM_I2C_MASTER        ((LPI2C_Type *)EEPROM_I2C_MASTER_BASE)

#define EEPROM_SIZE                         256
#define EEPROM_PAGE_SIZE                    8
#define EEPROM_ADDRESS_7_BIT                    (0xA0>>1)
#define EEPROM_WRITE_ADDRESS_8_BIT              (0xA0)
#define EEPROM_READ_ADDRESS_8_BIT               (0xA1)

#define EEPROM_INER_ADDRESS_SIZE            0x01

#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x100)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))


int32_t EEPROM_I2C_ReadBytes(uint8_t client_addr, uint8_t *buf, int32_t len);

void I2C_EEPROM_Init(void);
uint32_t I2C_WriteBytes(uint8_t ClientAddr,uint8_t* pBuffer,  uint8_t NumByteToWrite);
uint32_t I2C_ReadBytes(uint8_t ClientAddr,uint8_t* pBuffer, uint16_t NumByteToRead);

uint32_t I2C_EEPROM_BufferRead( uint8_t ClientAddr,
                                uint8_t ReadAddr,
                                uint8_t* pBuffer,
                                uint16_t NumByteToRead);

uint32_t I2C_EEPROM_Page_Write( uint8_t ClientAddr,
                                uint8_t WriteAddr,
                                uint8_t* pBuffer,
                                uint8_t NumByteToWrite);

void I2C_EEPROM_Buffer_Write( uint8_t ClientAddr,
                              uint8_t WriteAddr,
                              uint8_t* pBuffer,
                              uint16_t NumByteToWrite);


uint8_t I2C_EEPROM_WaitStandbyState(uint8_t ClientAddr);

uint8_t EEPROM_Test(void);


#endif /* BSP_I2C_EEPROM_H_ */
