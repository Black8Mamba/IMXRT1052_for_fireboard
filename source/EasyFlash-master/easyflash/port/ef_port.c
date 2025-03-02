/*
 * This file is part of the EasyFlash Library.
 *
 * Copyright (c) 2015-2019, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-01-16
 */

#include <easyflash.h>
#include <stdarg.h>
#include <string.h>
#include "fsl_debug_console.h"
#include "bsp_nand.h"
#include "fsl_nand_flash.h"
#include "fsl_semc_nand_flash.h"

/* default environment variables set for user */

static uint32_t boot_count = 0;
//static time_t boot_time[10] = {0, 1, 2, 3};
static const ef_env default_env_set[] = {
//      {   key  , value, value_len }，
        {"username", "jiyongjie", 0},   //类型为字符串的环境变量可以设定值的长度为 0 ，此时会在初始化时会自动检测其长度
//        {"password", "0824", 0},
//        {"boot_count", &boot_count, sizeof(boot_count)},  //整形
////        {"boot_time", &boot_time, sizeof(boot_time)},  //数组类型，其他类型使用方式类似
};

#if (USE_NAND)
nand_handle_t nandHandle;
extern nand_config_t nandConfig;
static uint8_t mem_writeBuffer[FLASH_PAGE_SIZE];
static uint8_t mem_readBuffer[FLASH_PAGE_SIZE] = {0};
#endif

/**
 * Flash port for hardware initialize.
 *
 * @param default_env default ENV set for user
 * @param default_env_size default ENV size
 *
 * @return result
 */
EfErrCode ef_port_init(ef_env const **default_env, size_t *default_env_size) {
    EfErrCode result = EF_NO_ERR;
    status_t status;

    *default_env = default_env_set;
    *default_env_size = sizeof(default_env_set) / sizeof(default_env_set[0]);
#if (USE_NAND)
    status = Nand_Flash_Init(&nandConfig, &nandHandle);
    if (status != kStatus_Success)
    {
        PRINTF("NAND Flash init failed\n");
        PRINTF("error code: %d \n", status);
        return EF_ENV_INIT_FAILED;
    }

    PRINTF("NAND FlashID:0x%x\n", NAND_ReadID());
#endif

    return result;
}

/**
 * Read data from flash.
 * @note This operation's units is word.
 *
 * @param addr flash address
 * @param buf buffer to store read data
 * @param size read bytes size
 *
 * @return result
 */
EfErrCode ef_port_read(uint32_t addr, uint32_t *buf, size_t size) {
    EfErrCode result = EF_NO_ERR;
    /* You can add your code under here. */
//    PRINTF("%s: addr:%x, size:%d\n", __func__, addr, size);
#if (USE_SDRAM)
    memcpy((void*)buf, (void*)addr, size);
#endif

#if (USE_NAND)
    size_t bytesInPage = nandHandle.bytesInPageDataArea;
    size_t offset = addr % bytesInPage;
    size_t remaining = size;
    uint8_t *buffer = (uint8_t *)buf;

    while (remaining > 0) {
    	int loop_read = 0;
        size_t readSize = bytesInPage - offset;
        if (readSize > remaining) {
            readSize = remaining;
        }

        status_t status = Nand_Flash_Read_Page(&nandHandle, addr / bytesInPage, mem_readBuffer, bytesInPage);
        if (status != kStatus_Success) {
            PRINTF("NAND Flash read failed\n");
            return EF_READ_ERR;
        }

        memcpy(buffer, mem_readBuffer + offset, readSize);

        addr += readSize;
        buffer += readSize;
        remaining -= readSize;
        offset = 0;
    }
#endif

    return EF_NO_ERR;
}

/**
 * Erase data on flash.
 * @note This operation is irreversible.
 * @note This operation's units is different which on many chips.
 *
 * @param addr flash address
 * @param size erase bytes size
 *
 * @return result
 */
EfErrCode ef_port_erase(uint32_t addr, size_t size) {
    EfErrCode result = EF_NO_ERR;
    /* make sure the start address is a multiple of EF_ERASE_MIN_SIZE */
    EF_ASSERT(addr % EF_ERASE_MIN_SIZE == 0);

#if (USE_SDRAM)
    memset((void*)addr, 0xFF, size);
#endif

#if (USE_NAND)
    size_t bytesInBlock = nandHandle.bytesInPageDataArea * nandHandle.pagesInBlock;
    size_t startBlock = addr / bytesInBlock;
    size_t endBlock = (addr + size - 1) / bytesInBlock;

    for (size_t block = startBlock; block <= endBlock; block++) {
        status_t status = Nand_Flash_Erase_Block(&nandHandle, block);
        if (status != kStatus_Success) {
            PRINTF("NAND Flash erase failed\n");
            return EF_ERASE_ERR;
        }
    }
#endif

    /* You can add your code under here. */

    return result;
}
/**
 * Write data to flash.
 * @note This operation's units is word.
 * @note This operation must after erase. @see flash_erase.
 *
 * @param addr flash address
 * @param buf the write data buffer
 * @param size write bytes size
 *
 * @return result
 */
EfErrCode ef_port_write(uint32_t addr, const uint32_t *buf, size_t size) {
    EfErrCode result = EF_NO_ERR;
    /* You can add your code under here. */
#if (USE_SDARM)
    memcpy((void*)addr, (void*)buf, size);
#endif

#if (USE_NAND)
    size_t bytesInPage = nandHandle.bytesInPageDataArea;
    size_t offset = addr % bytesInPage;
    size_t remaining = size;
    const uint8_t *buffer = (const uint8_t *)buf;

    while (remaining > 0) {
    	int loop = 0;
        size_t writeSize = bytesInPage - offset;
        if (writeSize > remaining) {
            writeSize = remaining;
        }

        if (offset != 0 || writeSize != bytesInPage) {
            status_t status = Nand_Flash_Read_Page(&nandHandle, addr / bytesInPage, mem_writeBuffer, bytesInPage);
            if (status != kStatus_Success) {
                PRINTF("NAND Flash read failed\n");
                return EF_READ_ERR;
            }
        }

        memcpy(mem_writeBuffer + offset, buffer, writeSize);

        status_t status = Nand_Flash_Page_Program(&nandHandle, addr / bytesInPage, mem_writeBuffer, bytesInPage);
        if (status != kStatus_Success) {
            PRINTF("NAND Flash write failed\n");
            return EF_WRITE_ERR;
        }

        addr += writeSize;
        buffer += writeSize;
        remaining -= writeSize;
        offset = 0;
    }

#endif

    return result;
}

/**
 * lock the ENV ram cache
 */
void ef_port_env_lock(void) {
    
    /* You can add your code under here. */
    
}

/**
 * unlock the ENV ram cache
 */
void ef_port_env_unlock(void) {
    
    /* You can add your code under here. */
    
}


/**
 * This function is print flash debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 *
 */
void ef_log_debug(const char *file, const long line, const char *format, ...) {

#ifdef PRINT_DEBUG

    va_list args;
    int result = 0;
    /* args point to the first variable parameter */
    va_start(args, format);
    result = DbgConsole_Vprintf(format, args);
    /* You can add your code under here. */
    
    va_end(args);

#endif

}

/**
 * This function is print flash routine info.
 *
 * @param format output format
 * @param ... args
 */
void ef_log_info(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);

    /* You can add your code under here. */
    
    va_end(args);
}
/**
 * This function is print flash non-package info.
 *
 * @param format output format
 * @param ... args
 */
void ef_print(const char *format, ...) {
    va_list args;
    int result = 0;
    /* args point to the first variable parameter */
    va_start(args, format);
    result = DbgConsole_Vprintf(format, args);
    /* You can add your code under here. */
    
    va_end(args);
}
