/************************************************************************************//**
* \file         Demo/_template/Boot/hooks.c
* \brief        Bootloader callback source file.
* \ingroup      Boot__template
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2019  by Feaser    http://www.feaser.com    All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 3 of the License, or (at your option) any later
* version.
*
* OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE. See the GNU General Public License for more details.
*
* You have received a copy of the GNU General Public License along with OpenBLT. It 
* should be located in ".\Doc\license.html". If not, contact Feaser to obtain a copy.
* 
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */
#include "blt_conf.h"
#include "bsp_led.h"                                 /* LED driver header                  */
#define LOG_TAG    "hook"
#define LOG_LVL    ELOG_LVL_DEBUG
#include "elog.h"
#include "bsp_led.h"
#include "perf_counter.h"
#include "mbedtls/md5.h"

static int enter_backdoor = 0;
/****************************************************************************************
*   B A C K D O O R   E N T R Y   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_BACKDOOR_HOOKS_ENABLE > 0)
/************************************************************************************//**
** \brief     Initializes the backdoor entry option.
** \return    none.
**
****************************************************************************************/
void BackDoorInitHook(void)
{
	log_i("BackDoorInitHook!");
} /*** end of BackDoorInitHook ***/


/************************************************************************************//**
** \brief     Checks if a backdoor entry is requested.
** \return    BLT_TRUE if the backdoor entry is requested, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool BackDoorEntryHook(void)
{
	log_i("BackDoorEntryHook!");
	enter_backdoor = 1;
  /* default implementation always activates the bootloader after a reset */
  return BLT_TRUE;
} /*** end of BackDoorEntryHook ***/
#endif /* BOOT_BACKDOOR_HOOKS_ENABLE > 0 */


/****************************************************************************************
*   C P U   D R I V E R   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_CPU_USER_PROGRAM_START_HOOK > 0)
/************************************************************************************//**
** \brief     Callback that gets called when the bootloader is about to exit and
**            hand over control to the user program. This is the last moment that
**            some final checking can be performed and if necessary prevent the
**            bootloader from activiting the user program.
** \return    BLT_TRUE if it is okay to start the user program, BLT_FALSE to keep
**            keep the bootloader active.
**
****************************************************************************************/
blt_bool CpuUserProgramStartHook(void)
{
	log_i("CpuUserProgramStartHook!");
  /* additional and optional backdoor entry through the pushbutton on the board. to
   * force the bootloader to stay active after reset, keep it pressed during reset.
   */
  /* TODO ##Boot Optionally configure an extra backdoor entry to force the bootloader
   * to stay active and not start the user program. For example if a digital input is
   * in a specific state.
   */ 
  if (1 == 0)
  {
    /* pushbutton pressed, so do not start the user program and keep the
     * bootloader active instead.
     */
    return BLT_FALSE;
  }

  /* clean up the LED driver */
//  LedBlinkExit();

  /* okay to start the user program */
  return BLT_TRUE;
} /*** end of CpuUserProgramStartHook ***/
#endif /* BOOT_CPU_USER_PROGRAM_START_HOOK > 0 */


/****************************************************************************************
*   W A T C H D O G   D R I V E R   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_COP_HOOKS_ENABLE > 0)
/************************************************************************************//**
** \brief     Callback that gets called at the end of the internal COP driver
**            initialization routine. It can be used to configure and enable the
**            watchdog.
** \return    none.
**
****************************************************************************************/
void CopInitHook(void)
{
	log_i("CopInitHook!");
  /* this function is called upon initialization. might as well use it to initialize
   * the LED driver. It is kind of a visual watchdog anyways.
   */
//  LedBlinkInit(100);
} /*** end of CopInitHook ***/


/************************************************************************************//**
** \brief     Callback that gets called at the end of the internal COP driver
**            service routine. This gets called upon initialization and during
**            potential long lasting loops and routine. It can be used to service
**            the watchdog to prevent a watchdog reset.
** \return    none.
**
****************************************************************************************/
void CopServiceHook(void)
{
//	log_i("CopServiceHook!");
  /* run the LED blink task. this is a better place to do it than in the main() program
   * loop. certain operations such as flash erase can take a long time, which would cause
   * a blink interval to be skipped. this function is also called during such operations,
   * so no blink intervals will be skipped when calling the LED blink task here.
   */
//  LedBlinkTask();
	int cycle = 200;
	if (enter_backdoor == 1)
	{
		cycle = 500;
	}

	if (get_system_ms() % cycle == 0)
	{
		RGB_LED_COLOR_RED;
	} else
	{
		RGB_LED_COLOR_BLACK;
	}

} /*** end of CopServiceHook ***/
#endif /* BOOT_COP_HOOKS_ENABLE > 0 */


/****************************************************************************************
*   N O N - V O L A T I L E   M E M O R Y   D R I V E R   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_NVM_HOOKS_ENABLE > 0)
/************************************************************************************//**
** \brief     Callback that gets called at the start of the internal NVM driver
**            initialization routine. 
** \return    none.
**
****************************************************************************************/
void NvmInitHook(void)
{
	log_i("NvmInitHook!");
} /*** end of NvmInitHook ***/


/************************************************************************************//**
** \brief     Callback that gets called at the start of the NVM driver write 
**            routine. It allows additional memory to be operated on. If the address
**            is not within the range of the additional memory, then 
**            BLT_NVM_NOT_IN_RANGE must be returned to indicate that the data hasn't
**            been written yet.
** \param     addr Start address.
** \param     len  Length in bytes.
** \param     data Pointer to the data buffer.
** \return    BLT_NVM_OKAY if successful, BLT_NVM_NOT_IN_RANGE if the address is
**            not within the supported memory range, or BLT_NVM_ERROR is the write
**            operation failed.
**
****************************************************************************************/
blt_int8u NvmWriteHook(blt_addr addr, blt_int32u len, blt_int8u *data)
{
	log_i("NvmWriteHook, addr:%x, len:%x!", addr, len);
//  return BLT_NVM_NOT_IN_RANGE;
	return BLT_NVM_OKAY;
} /*** end of NvmWriteHook ***/


/************************************************************************************//**
** \brief     Callback that gets called at the start of the NVM driver erase 
**            routine. It allows additional memory to be operated on. If the address
**            is not within the range of the additional memory, then
**            BLT_NVM_NOT_IN_RANGE must be returned to indicate that the memory
**            hasn't been erased yet.
** \param     addr Start address.
** \param     len  Length in bytes.
** \return    BLT_NVM_OKAY if successful, BLT_NVM_NOT_IN_RANGE if the address is
**            not within the supported memory range, or BLT_NVM_ERROR is the erase
**            operation failed.
**
****************************************************************************************/
blt_int8u NvmEraseHook(blt_addr addr, blt_int32u len)
{
	log_i("NvmEraseHook, addr:%x, len:%x!", addr, len);
//  return BLT_NVM_NOT_IN_RANGE;
	return BLT_NVM_OKAY;
} /*** end of NvmEraseHook ***/


/************************************************************************************//**
** \brief     Callback that gets called at the end of the NVM programming session.
** \return    BLT_TRUE is successful, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool NvmDoneHook(void)
{
	log_i("NvmDoneHook!");
  return BLT_TRUE;
} /*** end of NvmDoneHook ***/
#endif /* BOOT_NVM_HOOKS_ENABLE > 0 */


#if (BOOT_NVM_CHECKSUM_HOOKS_ENABLE > 0)
/************************************************************************************//**
** \brief     Verifies the checksum, which indicates that a valid user program is
**            present and can be started.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool NvmVerifyChecksumHook(void)
{
	log_i("NvmVerifyChecksumHook!");
  return BLT_TRUE;
} /*** end of NvmVerifyChecksum ***/

extern unsigned char md5_area_start[];
extern unsigned char md5_area_end[];
// 使用举例
blt_bool clear_md5_area(void) {
    status_t status = FlexSPI_NorFlash_Erase_Sector(FLEXSPI, md5_area_start-0x60000000, md5_area_end-md5_area_start);
    if (status != kStatus_Success)
    {
        log_e("FlexSPI_NorFlash_Erase_Block failed!\r\n");
        return BLT_FALSE;
    }
    
    return BLT_TRUE;
}
extern uint32_t recv_size;
#define BLOCK_SIZE 528
uint8_t block[BLOCK_SIZE];
blt_bool write_md5sum(void)
{
    /* TODO ##Port Implement the MD5 checksum verification logic here.
     * This function should read the MD5 checksum from the non-volatile memory
     * and compare it with the expected checksum of the user program.
     */
    log_i("verify_md5sum!\r\n");
    
    int count = recv_size / BLOCK_SIZE;
    int ret = 0;

    mbedtls_md5_context ctx;
    mbedtls_md5_init(&ctx);
    mbedtls_md5_starts(&ctx);

    uint8_t md5[256] = {0};

    for (int i = 0; i < count; ++i)
    {
    	memcpy(block, 0x60040000+i*BLOCK_SIZE, BLOCK_SIZE);
    	ret = mbedtls_md5_update(&ctx, block, BLOCK_SIZE);
    	if (ret != 0)
    	{
    		log_e("mbedtls_md5_update failed!\r\n");
    		return BLT_FALSE;
    	}
    }

    ret = mbedtls_md5_finish(&ctx, md5);
    if (ret != 0)
    {
		log_e("mbedtls_md5_finish failed!\r\n");
		return BLT_FALSE;
    }

    for (int i = 0; i < 16; ++i)
    {
    	elog_raw("%02x ", md5[i]);
    }
    elog_raw("\r\n");

	  status_t status = FlexSPI_NorFlash_Page_Program(FLEXSPI, md5_area_start-0x60000000, md5, 256);
	  if (status != kStatus_Success)
	  {
		  log_e("FlexSPI_NorFlash_Page_Program failed!\r\n");
		  return BLT_FALSE;
	  }

    // Example: Always return true for now
    return BLT_TRUE;
} /*** end of verify_md5sum ***/

/************************************************************************************//**
** \brief     Writes a checksum of the user program to non-volatile memory. This is
**            performed once the entire user program has been programmed. Through
**            the checksum, the bootloader can check if a valid user programming is
**            present and can be started.
** \return    BLT_TRUE if successful, BLT_FALSE otherwise. 
**
****************************************************************************************/
blt_bool NvmWriteChecksumHook(void)
{
    log_i("NvmWriteChecksumHook, addr:%x\r\n", md5_area_start);

    blt_bool program_last_block(void);
    program_last_block();

    clear_md5_area();
    write_md5sum();
    return BLT_TRUE;
}
#endif /* BOOT_NVM_CHECKSUM_HOOKS_ENABLE > 0 */


/****************************************************************************************
*   S E E D / K E Y   S E C U R I T Y   H O O K   F U N C T I O N S
****************************************************************************************/

#if (BOOT_XCP_SEED_KEY_ENABLE > 0)
/************************************************************************************//**
** \brief     Provides a seed to the XCP master that will be used for the key 
**            generation when the master attempts to unlock the specified resource. 
**            Called by the GET_SEED command.
** \param     resource  Resource that the seed if requested for (XCP_RES_XXX).
** \param     seed      Pointer to byte buffer wher the seed will be stored.
** \return    Length of the seed in bytes.
**
****************************************************************************************/
blt_int8u XcpGetSeedHook(blt_int8u resource, blt_int8u *seed)
{
  /* request seed for unlocking ProGraMming resource */
  if ((resource & XCP_RES_PGM) != 0)
  {
    seed[0] = 0x55;
  }

  /* return seed length */
  return 1;
} /*** end of XcpGetSeedHook ***/


/************************************************************************************//**
** \brief     Called by the UNLOCK command and checks if the key to unlock the 
**            specified resource was correct. If so, then the resource protection 
**            will be removed.
** \param     resource  resource to unlock (XCP_RES_XXX).
** \param     key       pointer to the byte buffer holding the key.
** \param     len       length of the key in bytes.
** \return    1 if the key was correct, 0 otherwise.
**
****************************************************************************************/
blt_int8u XcpVerifyKeyHook(blt_int8u resource, blt_int8u *key, blt_int8u len)
{
  /* suppress compiler warning for unused parameter */
  len = len;

  /* the example key algorithm in "libseednkey.dll" works as follows:
   *  - PGM will be unlocked if key = seed - 1
   */

  /* check key for unlocking ProGraMming resource */
  if ((resource == XCP_RES_PGM) && (key[0] == (0x55-1)))
  {
    /* correct key received for unlocking PGM resource */
    return 1;
  }

  /* still here so key incorrect */
  return 0;
} /*** end of XcpVerifyKeyHook ***/
#endif /* BOOT_XCP_SEED_KEY_ENABLE > 0 */


/*********************************** end of hooks.c ************************************/
