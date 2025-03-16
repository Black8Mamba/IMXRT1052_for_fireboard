/*
 * lfs_app.c
 *
 *  Created on: 2025年3月16日
 *      Author: yongjie
 */

#include "lfs.h"
#include "fsl_flexspi.h"
#include "bsp_norflash.h"
#include "norflash_partition.h"

lfs_t lfs;
lfs_file_t file;
lfs_dir_t dir;

//https://blog.csdn.net/weixin_43908815/article/details/130179573 插件

static uint8_t read_buf[256];
static uint8_t prog_buf[256];
static uint8_t lookahead_buf[256];
static uint8_t cache_buf[256];

#define START_ADDR 	(4096*4096)

int user_provided_block_device_read(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, void *buffer, lfs_size_t size)
{
    status_t status = FlexSPI_NorFlash_Buffer_Read(FLEXSPI,
    		LITTLE_FS_START_ADDR+block*SECTOR_SIZE+off,
			(uint8_t*)buffer,
			(uint16_t)size);

    if (status != kStatus_Success)
    {
        PRINTF("FlexSPI_NorFlash_Buffer_Read failed, addr:%x, size:%x!\r\n", LITTLE_FS_START_ADDR+block*SECTOR_SIZE+off, size);
        return LFS_ERR_IO;
    }

	return LFS_ERR_OK;
}

int user_provided_block_device_prog(const struct lfs_config *c, lfs_block_t block,
        lfs_off_t off, const void *buffer, lfs_size_t size)
{
    status_t status = FlexSPI_NorFlash_Buffer_Program(FLEXSPI,
    		LITTLE_FS_START_ADDR+block*SECTOR_SIZE+off,
			(uint8_t*)buffer,
			(uint16_t)size);

    if (status != kStatus_Success)
    {
        PRINTF("FlexSPI_NorFlash_Buffer_Program failed, addr:%x, size:%x!\r\n", LITTLE_FS_START_ADDR+block*SECTOR_SIZE+off, size);
        return LFS_ERR_IO;
    }

	return LFS_ERR_OK;
}

int user_provided_block_device_erase(const struct lfs_config *c, lfs_block_t block)
{
    status_t status = FlexSPI_NorFlash_Erase_Sector(FLEXSPI, LITTLE_FS_START_ADDR+block*SECTOR_SIZE);
    if (status != kStatus_Success)
    {
        PRINTF("FlexSPI_NorFlash_Erase_Sector failed, addr:%x!\r\n", LITTLE_FS_START_ADDR+block*SECTOR_SIZE);
        return LFS_ERR_IO;
    }
	return LFS_ERR_OK;
}

int user_provided_block_device_sync(const struct lfs_config *c)
{
	return LFS_ERR_OK;
}

const struct lfs_config lfs_w25q256_cfg = {
    // block device operations
    .read  = user_provided_block_device_read,
    .prog  = user_provided_block_device_prog,
    .erase = user_provided_block_device_erase,
    .sync  = user_provided_block_device_sync,

    // block device configuration
    .read_size = 256,
    .prog_size = 256,
    .block_size = 4096,
    .block_count = 8192,
    .cache_size = 256,
    .lookahead_size = 256,
    .block_cycles = 500,
	.read_buffer = (void*)read_buf,
	.prog_buffer = (void*)prog_buf,
	.lookahead_buffer = (void*)lookahead_buf,
};

const struct lfs_file_config file_cfg = {
		.buffer = cache_buf,
};

int init_little_fs(void) {
    // mount the filesystem
    int err = lfs_mount(&lfs, &lfs_w25q256_cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        lfs_format(&lfs, &lfs_w25q256_cfg);
        lfs_mount(&lfs, &lfs_w25q256_cfg);
    }
#if 0

    // read current count
    uint32_t boot_count = 0;
    lfs_file_opencfg(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT, &file_cfg);

    lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

    // update boot count
    boot_count += 1;
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs, &file);

    // release any resources we were using
    lfs_unmount(&lfs);

    // print the boot count
    PRINTF("boot_count: %d\n", boot_count);
#endif

    return 0;
}


