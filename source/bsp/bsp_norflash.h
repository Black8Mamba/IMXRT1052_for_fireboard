/*
 * bsp_norflash.h
 *
 *  Created on: 2025年3月15日
 *      Author: yongjie
 */

#ifndef BSP_NORFLASH_H_
#define BSP_NORFLASH_H_

#include "fsl_common.h"
#include "fsl_debug_console.h"

#define FLASH_SIZE          (32*1024) //32M
#ifdef FLASH_PAGE_SIZE
#undef FLASH_PAGE_SIZE
#endif
#define FLASH_PAGE_SIZE     256 //256bytes
#define SECTOR_SIZE         (4*1024) //4k

#define FLASH_ADDR_LENGTH    32 // 4bytes addr

#define W25Q_WriteEnable                0x06
#define W25Q_WriteDisable               0x04
#define W25Q_ReadStatusReg              0x05
#define W25Q_WriteStatusReg             0x01
#define W25Q_ReadData                   0x03
#define W25Q_ReadData_4Addr             0x13
#define W25Q_FastReadData               0x0B
#define W25Q_FastReadData_4Addr         0x0C
#define W25Q_FastReadDual               0x3B
#define W25Q_FastReadDual_4Addr         0x3C
#define W25Q_FastReadQuad               0x6B
#define W25Q_FastReadQuad_4Addr         0xEC
#define W25Q_PageProgram                0x02
#define W25Q_PageProgram_4Addr          0x12
#define W25Q_PageProgramQuad            0x32
#define W25Q_PageProgramQuad_4Addr      0x34
#define W25Q_BlockErase                 0xD8
#define W25Q_BlockErase_4Addr           0xDC
#define W25Q_SectorErase                0x20
#define W25Q_SectorErase_4Addr          0x21
#define W25Q_ChipErase                  0xC7
#define W25Q_PowerDown                  0xB9
#define W25Q_ReleasePowerDown           0xAB
#define W25Q_DeviceID                   0xAB
#define W25Q_ManufactDeviceID           0x90
#define W25Q_JedecDeviceID              0x9F

#define FLASH_ID                        0X18
#define FLASH_WINBOND_JEDECDEVICE_ID    0XEF4019
#define FLASH_ISSI_JEDECDEVICE_ID       0X9D6019

#define FLASH_BUSY_STATUS_POL       1
#define FLASH_BUSY_STATUS_OFFSET    0

#define NORFLASH_AHB_POINTER(addr)              (void*)(FlexSPI_AMBA_BASE + addr)
#define NORFLASH_AHB_READ_BYTE(addr)            *(uint8_t *)NORFLASH_AHB_POINTER(addr)
#define NORFLASH_AHB_WRITE_BYTE(addr, data)     *(uint8_t *)NORFLASH_AHB_POINTER(addr) = data
#define NORFLASH_AHB_READ_HALFWORD(addr)        *(uint16_t *)NORFLASH_AHB_POINTER(addr)
#define NORFLASH_AHB_WRITE_HALFWORD(addr, data) *(uint16_t *)NORFLASH_AHB_POINTER(addr) = data
#define NORFLASH_AHB_READ_WORD(addr)            *(uint32_t *)NORFLASH_AHB_POINTER(addr)
#define NORFLASH_AHB_WRITE_WORD(addr, data)     *(uint32_t *)NORFLASH_AHB_POINTER(addr) = data

#define FLASH_DEBUG_ON             1
#define FLASH_DEBUG_ARRAY_ON       1
#define FLASH_DEBUG_FUNC_ON        1

#define FLASH_INFO(fmt,arg...)           PRINTF("<<-FLASH-INFO->> "fmt"\r\n",##arg)
#define FLASH_ERROR(fmt,arg...)          PRINTF("<<-FLASH-ERROR->> "fmt"\r\n",##arg)
#define FLASH_DEBUG(fmt,arg...)          do{\
                                         if(FLASH_DEBUG_ON)\
                                         PRINTF("<<-FLASH-DEBUG->> [%d]"fmt"\r\n",__LINE__, ##arg);\
                                          }while(0)

#define FLASH_DEBUG_ARRAY(array, num)    do{\
                                             int32_t i;\
                                             uint8_t* a = array;\
                                             if(FLASH_DEBUG_ARRAY_ON)\
                                             {\
                                                PRINTF("<<-FLASH-DEBUG-ARRAY->>\r\n");\
                                                for (i = 0; i < (num); i++)\
                                                {\
                                                    PRINTF("%02x   ", (a)[i]);\
                                                    if ((i + 1 ) %10 == 0)\
                                                    {\
                                                        PRINTF("\r\n");\
                                                    }\
                                                }\
                                                PRINTF("\r\n");\
                                            }\
                                          }while(0)

#define FLASH_DEBUG_FUNC()             do{\
                                           if(FLASH_DEBUG_FUNC_ON)\
                                           PRINTF("<<-FLASH-FUNC->> Func:%s@Line:%d\r\n",__func__,__LINE__);\
                                          }while(0)

int FlexSPI_NorFlash_Init(void);
status_t FlexSPI_NorFlash_Get_JedecDevice_ID(FLEXSPI_Type *base, uint32_t *vendorID);
status_t FlexSPI_NorFlash_Get_Device_ID(FLEXSPI_Type *base, uint8_t *vendorID);
status_t FlexSPI_NorFlash_Write_Enable(FLEXSPI_Type *base);
status_t FlexSPI_NorFlash_Erase_Sector(FLEXSPI_Type *base, uint32_t dstAddr);
status_t FlexSPI_NorFlash_Erase_Block(FLEXSPI_Type *base, uint32_t dstAddr);
status_t FlexSPI_NorFlash_Wait_Bus_Busy(FLEXSPI_Type *base);
status_t FlexSPI_NorFlash_Write_Status_Register(FLEXSPI_Type *base, uint8_t *config);
status_t FlexSPI_NorFlash_Read_Status_Register(FLEXSPI_Type *base, uint8_t *SR_value);
status_t FlexSPI_NorFlash_Enable_Quad_Mode(FLEXSPI_Type *base);

status_t FlexSPI_NorFlash_Page_Program(FLEXSPI_Type *base,
                                       uint32_t dstAddr,
                                       uint8_t *src,
                                       uint16_t dataSize);

status_t FlexSPI_NorFlash_Buffer_Program(FLEXSPI_Type *base,
                                         uint32_t dstAddr,
                                         uint8_t *src,
                                         uint16_t dataSize);

status_t FlexSPI_NorFlash_Buffer_Read(FLEXSPI_Type *base,
                                      uint32_t address,
                                      uint8_t *dst,
                                      uint16_t dataSize);

#endif /* BSP_NORFLASH_H_ */
