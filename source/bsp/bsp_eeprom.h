/*
 * bsp_eeprom.h
 *
 *  Created on: 2025年3月31日
 *      Author: yongjie
 */

#include "fsl_lpi2c.h"
#include "fsl_adapter_i2c.h"

#ifndef BSP_BSP_EEPROM_H_
#define BSP_BSP_EEPROM_H_

#define EEPROM_SIZE                         	256
#define EEPROM_PAGE_SIZE                    	8
#define EEPROM_ADDRESS_7_BIT                    (0xA0>>1)
#define EEPROM_WRITE_ADDRESS_8_BIT              (0xA0)
#define EEPROM_READ_ADDRESS_8_BIT               (0xA1)
#define EEPROM_INER_ADDRESS_SIZE 				0x01

#endif /* BSP_BSP_EEPROM_H_ */
