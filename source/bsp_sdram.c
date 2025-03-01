/*
 * bsp_sdram.c
 *
 *  Created on: 2025年2月25日
 *      Author: yongjie
 */
#include "fsl_debug_console.h"
#include "fsl_iomuxc.h"
#include "fsl_semc.h"
#include "bsp_sdram.h"

/**
* @brief  ³õÊ¼»¯SDRAMÏà¹ØµÄSEMCÅäÖÃ
* @param  ÎÞ
* @retval ÎÞ
*/
static status_t SDRAM_SEMC_Config(void)
{
    semc_config_t config;
    semc_sdram_config_t sdramconfig;
    uint32_t clockFrq = EXAMPLE_SEMC_CLK_FREQ;

    /* ½á¹¹ÌåµÄÄÚÈÝÈ«²¿ÇåÁã */
    memset(&config, 0, sizeof(semc_config_t));
    memset(&sdramconfig, 0, sizeof(semc_sdram_config_t));

    /* Ê¹ÓÃÄ¬ÈÏÅäÖÃ³õÊ¼»¯ SEMC. */
    SEMC_GetDefaultConfig(&config);

    /* £¡×¢Òâ£¡´Ë´¦±ØÐëÊ¹ÓÃÕâ¸öÄ£Ê½£¬
       ·ñÔòSDRAMÇý¶¯Ê±ÖÓ²»ÄÜÅÜµ½¸ßÆµÂÊ */
    config.dqsMode = kSEMC_Loopbackdqspad;
    SEMC_Init(SEMC, &config);

    /* ÅäÖÃSDRAM */
    /* Ñ¡ÔñÆ¬Ñ¡Òý½Å£¬±¾Ó¦ÓÃÊ¹ÓÃCS0£¬´ËÅäÖÃÎÞÐ§ */
    sdramconfig.csxPinMux = kSEMC_MUXCSX0;
    /* ±¾SDRAMÓ³ÉäµÄ»ùµØÖ· */
    sdramconfig.address = 0x80000000;
    /* SDRAMµÄÈÝÁ¿´óÐ¡£¬µ¥Î»ÎªKBytes, 32MB = 32*1024*1KBytes*/
    sdramconfig.memsize_kbytes = 32 * 1024;
    /* SDRAMµÄÊý¾ÝÏß¿í¶È */
    sdramconfig.portSize = kSEMC_PortSize16Bit;
    /* Í»·¢¶ÁÐ´µÄ×î´ó³¤¶È */
    sdramconfig.burstLen = kSEMC_Sdram_BurstLen8;
    /* ÁÐµØÖ·¿í¶È */
    sdramconfig.columnAddrBitNum = kSEMC_SdramColunm_9bit;
    /* CAS Latency */
    sdramconfig.casLatency = kSEMC_LatencyTwo;
    /* Ô¤³äµçÖÁÐÐÓÐÐ§µÄÊ±¼ä Trp */
    sdramconfig.tPrecharge2Act_Ns = 15;
    /* ÐÐÓÐÐ§ÖÁ¶ÁÐ´µÄÊ±¼ä Trcd */
    sdramconfig.tAct2ReadWrite_Ns = 15;
    /* Ë¢ÐÂ»Ö¸´Ê±¼ä£¬Ê¹ÓÃTrfc¡¢TxsrÖÐ½Ï´óµÄÒ»¸öÖµ */
    sdramconfig.tRefreshRecovery_Ns = 67;
    /* Ð´»Ö¸´Ê±¼ä£¬2 ¸öTclkÊ±ÖÓ */
    sdramconfig.tWriteRecovery_Ns = 2 * (1000000000 / clockFrq);
    /* CKE off µÄ×îÐ¡Ê±¼ä£¬1 ¸öTclkÊ±ÖÓ */
    sdramconfig.tCkeOff_Ns = 1 * (1000000000 / clockFrq);
    /* ÐÐÓÐÐ§ÖÁÔ¤³äµçÊ±¼ä£¬Tras */
    sdramconfig.tAct2Prechage_Ns = 42;
    /* ×ÔË¢ÐÂµÄ»Ö¸´Ê±¼ä£¬È¡tRefreshRecovery_NsÍ¬ÑùµÄÖµ */
    sdramconfig.tSelfRefRecovery_Ns = 67;
    /* Á½¸öË¢ÐÂÃüÁîÖ®¼äµÄÊ±¼ä */
    sdramconfig.tRefresh2Refresh_Ns = 60;
    /* Á½¸öÐÐÓÐÐ§ÃüÁîÖ®¼äµÄÊ±¼ä */
    sdramconfig.tAct2Act_Ns = 60;
    /* ·ÖÆµÆ÷¶¨Ê±ÖÜÆÚ£¬160¸öTclkÊ±ÖÓ */
    sdramconfig.tPrescalePeriod_Ns = 160 * (1000000000 / clockFrq);
    /* Ë¢ÐÂÃüÁîµÄÖÜÆÚ£¬¼´¶à¾ÃË¢ÐÂÒ»ÐÐ£¬64ms/8192  */
    sdramconfig.refreshPeriod_nsPerRow = 64 * 1000000 / 8192;
    /* ½ô¼±Ë¢ÐÂãÐÖµ */
    sdramconfig.refreshUrgThreshold = sdramconfig.refreshPeriod_nsPerRow;
    /* Ë¢ÐÂÃüÁîµÄÍ»·¢³¤¶È */
    sdramconfig.refreshBurstLen = 1;

    /* Ê¹ÓÃÒÔÉÏÅäÖÃ³õÊ¼»¯£¬²¢Ê¹ÓÃCS0£¬¼´ÓÃÔÚRegion 0ÇøÓò */
    return SEMC_ConfigureSDRAM(SEMC, kSEMC_SDRAM_CS0, &sdramconfig, clockFrq);
}

status_t SDRAM_Init(void)
{
  return SDRAM_SEMC_Config();
}
