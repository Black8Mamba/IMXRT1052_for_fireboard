/*
 * bsp_nand.h
 *
 *  Created on: 2025年3月1日
 *      Author: yongjie
 */

#ifndef BSP_NAND_H_
#define BSP_NAND_H_

#include "fsl_common.h"

#define EXAMPLE_SEMC SEMC
#define EXAMPLE_SEMC_START_ADDRESS (0x80000000U)
#define EXAMPLE_SEMC_CLK_FREQ CLOCK_GetFreq(kCLOCK_SemcClk)
#define EXAMPLE_SEMC_NAND_AXI_START_ADDRESS (0x9E000000U)
#define EXAMPLE_SEMC_NAND_IPG_START_ADDRESS (0x00000000U)
#ifdef FLASH_PAGE_SIZE
#undef FLASH_PAGE_SIZE
#endif
#define FLASH_PAGE_SIZE (4096)

void BOARD_InitNand(void);

#endif /* BSP_NAND_H_ */
