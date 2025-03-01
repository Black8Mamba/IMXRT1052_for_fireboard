/*
 * bsp_sdram.h
 *
 *  Created on: 2025年2月25日
 *      Author: yongjie
 */

#ifndef BSP_SDRAM_H_
#define BSP_SDRAM_H_

#include "fsl_common.h"

/* SEMC_CLK_ROOTÆµÂÊ */
#define EXAMPLE_SEMC_CLK_FREQ         CLOCK_GetFreq(kCLOCK_SemcClk)

/*******************************************************************************
 * º¯ÊýÉùÃ÷
 ******************************************************************************/
status_t SDRAM_Init(void);

void SEMC_SDRAMReadWrite32Bit(void);
void SEMC_SDRAMReadWrite16Bit(void);
void SEMC_SDRAMReadWrite8Bit(void);

#endif /* BSP_SDRAM_H_ */
