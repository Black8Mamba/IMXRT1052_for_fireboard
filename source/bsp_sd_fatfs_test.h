/*
 * bsp_sd_fatfs_test.h
 *
 *  Created on: 2025年3月8日
 *      Author: yongjie
 */

#ifndef BSP_SD_FATFS_TEST_H_
#define BSP_SD_FATFS_TEST_H_

#include "fsl_common.h"
#include "fsl_sd.h"


#include "ff.h"
#include "diskio.h"
#include "fsl_sd_disk.h"

FRESULT f_mount_test(FATFS* fileSystem);
FRESULT f_mkdir_test(char* dir);
FRESULT f_touch_test(const char* dir);
FRESULT f_open_test(char* dir,FIL* fileObject);
FRESULT f_close_test(FIL* fileObject);
FRESULT f_readdir_test(char* dir,DIR* directory,FILINFO* fileInformation);
FRESULT f_write_read_test(char* dir,  void* data_write, void* data_read);

#endif /* BSP_SD_FATFS_TEST_H_ */
