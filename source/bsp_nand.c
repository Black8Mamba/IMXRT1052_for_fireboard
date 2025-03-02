/*
 * bsp_namd.c
 *
 *  Created on: 2025年3月1日
 *      Author: yongjie
 */
#include "fsl_debug_console.h"
#include "fsl_iomuxc.h"
#include "fsl_semc.h"

#include "bsp_nand.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "bsp_nand.h"
#include "fsl_nand_flash.h"
#include "fsl_semc_nand_flash.h"
#include "perf_counter/perf_counter.h"


semc_nand_config_t semcNandConfig = {
    .cePinMux = kSEMC_MUXCSX0,     /*!< ÉèÖÃÆ¬Ñ¡ */
    .axiAddress = EXAMPLE_SEMC_NAND_AXI_START_ADDRESS,  /*!< »ùµØÖ· */
    .axiMemsize_kbytes = 2 * 1024 * 1024,     /*!< AXI½Ó¿Úflash´óÐ¡ 8*1024*2*1024*1024 = 16Gb. */
    .ipgAddress = EXAMPLE_SEMC_NAND_IPG_START_ADDRESS,  /*!< The base address. */
    .ipgMemsize_kbytes = 2 * 1024 * 1024,     /*!< IPG½Ó¿Úflash´óÐ¡ 8*1024*2*1024*1024 = 16Gb. */
    .rdyactivePolarity = kSEMC_RdyActiveLow,    /*!< RDY¼«ÐÔ */
    .arrayAddrOption = kSEMC_NandAddrOption_5byte_CA2RA3,
    .edoModeEnabled = false,                   /*!< µØÖ·Ä£Ê½ */
    .columnAddrBitNum = kSEMC_NandColum_12bit,
    .burstLen = kSEMC_Nand_BurstLen1,        /*!< Í»·¢³¤¶È */
    .portSize = kSEMC_PortSize8Bit,         /*!< ½Ó¿ÚÎ»¿í */
    .timingConfig = NULL,
};

void delay_nand_us(uint32_t us)
{
	delay_us(us);
}

semc_mem_nand_config_t semcMemConfig = {
    .semcNandConfig = &semcNandConfig,/*¿ØÖÆÆ÷ÅäÖÃ½á¹¹Ìå. */
    .delayUS = delay_nand_us,                  /*Íâ²¿Ìá¹©µÄÑÓÊ±º¯Êý */
    .onfiVersion = kNandOnfiVersion_1p0,/*½öÖ§³Öonfi nand flash  */
    .readyCheckOption = kNandReadyCheckOption_SR,/*½öÖ§³Öonfi nand flash  */
    .eccCheckType = kNandEccCheckType_DeviceECC,/*Ñ¡ÔñÐ£ÑéÀàÐÍ*/
};

nand_config_t nandConfig = {
	/*ÄÚ´æ¿ØÖÆÆ÷ÅäÖÃÓ¦·ÖÅäÌØ¶¨µÄ¿ØÖÆÆ÷ÅäÖÃ½á¹¹Ö¸Õë*/
    .memControlConfig = (void *)&semcMemConfig,
    .driverBaseAddr = (void *)EXAMPLE_SEMC, /*Çý¶¯»ùµØÖ·. */
};

void BOARD_InitNand(void)
{
	semc_config_t config;

	/*³õÊ¼»¯nand_flashÏà¹ØÒý½Å*/
//	NAND_IOMUXC_MUX_Config();
//	NAND_IOMUXC_PAD_Config();

	/* ²ÎÊýÖÃÁã */
	memset(&config, 0, sizeof(semc_config_t));
	/* ³õÊ¼»¯SEMC. */
	SEMC_GetDefaultConfig(&config);
	SEMC_Init(SEMC, &config);
	/* ÎªNANDÉÁ´æ¿ØÖÆÆ÷Ê¹ÓÃÉèÖÃSEMCÊ±ÖÓÔ´ */
	semcMemConfig.clkSrc_Hz = EXAMPLE_SEMC_CLK_FREQ;
}
