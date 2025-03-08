/*
 * bsp_sd_fatfs_test.c
 *
 *  Created on: 2025年3月8日
 *      Author: yongjie
 */

#include "fsl_debug_console.h"

#include "fsl_sd.h"
#include "bsp_sd_fatfs_test.h"
#include "fsl_sdmmc_host.h"

FRESULT f_mount_test(FATFS* fileSystem)
{
  FRESULT error = FR_OK;
  BYTE work[FF_MAX_SS];
  const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};
  FATFS* g_fileSystem = fileSystem;

  error = f_mount(g_fileSystem, driverNumberBuffer, 1U);
  if (error)
  {
	  PRINTF("error!\n");
	  if (error == FR_NO_FILESYSTEM)
	  {
		  PRINTF("format sdcard!\n");
		  MKFS_PARM fmt_opt = {FM_FAT32, 0, 0, 0, 0};
		  error = f_mkfs("0:", &fmt_opt, work, sizeof(work));
          if (error)
          {
        	  PRINTF("format sdcard failed!\n");
        	  while(1);
          } else
          {
        	  PRINTF("format sdcard success!\n");
              error = f_mount(g_fileSystem, driverNumberBuffer, 0U);
              if(error)
              {
                PRINTF("mount fatfs failed!\n");
                while(1);
              }
              else
              {
            	  PRINTF("mount fatfs success!\n");
              }
          }
	  } else
	  {
		  PRINTF("error:%d\n", error);
	  }
  } else
  {
	  PRINTF("mount fatfs success!\n");
  }

  return error;
}

FRESULT f_mkdir_test(char* dir)
{
  FRESULT error;
  PRINTF("\r\n mkdir :%s\r\n",dir);
  error = f_mkdir(_T(dir));
  if (error)
  {
    if (error == FR_EXIST)
    {
      PRINTF("dir exist\r\n");
    }
    else
    {
      PRINTF("mkdir failed\r\n");
      return error;
    }
  }
  else
  {
    PRINTF("mkdir success\r\n");
  }
  return error;
}

FRESULT f_touch_test(const char* dir)
{
  FRESULT error = FR_OK;
  FIL g_fileObject ;   /* File object */
  error = f_open(&g_fileObject, dir, FA_CREATE_NEW);
  if (error)
  {
    if (error == FR_EXIST)
    {
      PRINTF("file:%s exist!\n", dir);
      return error;
    }
    else
    {
      PRINTF("touch file:%s failed!\n", dir);
      return error;
    }

  }
  else
  {
	  PRINTF("touch file:%s success!\n", dir);
  }

  error = f_close(&g_fileObject);
  if (error)
  {
	  PRINTF("close file %s failed!\n", dir);
	  return error;
  }
  PRINTF("close file:%s success!\n", dir);
  return error;

}

FRESULT f_open_test(char* dir,FIL* fileObject)
{
  FRESULT error = FR_OK;
  FIL* g_fileObject = fileObject;   /* File object */
  error = f_open(g_fileObject, _T(dir), (FA_WRITE | FA_READ ));
  if (error)
  {
    if (error == FR_EXIST)
    {
      PRINTF("file:%s exist\r\n", dir);
    }
  }
  else
  {
    PRINTF("open file:%s failed!\r\n", dir);
  }

  return error;
}

FRESULT f_readdir_test(char* dir,DIR* directory,FILINFO* fileInformation)
{
  FRESULT error = FR_OK;
  DIR* g_directory = directory; /* Directory object */
  FILINFO* g_fileInformation = fileInformation;


  PRINTF("list dir:%s\r\n", dir);

  error = f_opendir(g_directory, dir);
  if (error)
  {
    PRINTF("f_opendir failed:%s!\n", dir);
    return error;
  }

  for (;;)
  {
    error = f_readdir(g_directory, g_fileInformation);

    /* To the end. */
    if ((error != FR_OK) || (g_fileInformation->fname[0U] == 0U))
    {
      break;
    }
    if (g_fileInformation->fname[0] == '.')
    {
      continue;
    }
    if (g_fileInformation->fattrib & AM_DIR)
    {
      PRINTF("dir : %s\r\n", g_fileInformation->fname);
    }
    else
    {
      PRINTF("file : %s\r\n", g_fileInformation->fname);
    }
  }
  return error;
}

FRESULT f_write_read_test(char* dir, void* data_write, void* data_read)
{
  FIL g_fileObject ;
  void* g_data_write = data_write;
  void* g_data_read = data_read;
  UINT g_bytesWritten;
  UINT g_bytesRead;
  FRESULT error = FR_OK;

  f_open_test(dir,&g_fileObject);

  error = f_write(&g_fileObject, g_data_write, sizeof(g_data_write), &g_bytesWritten);
  if ((error) || (g_bytesWritten != sizeof(g_data_write)))
  {
    PRINTF("write file failed. \r\n");
  }
  else
  {
    PRINTF("write file success\r\n");
  }

  if (f_lseek(&g_fileObject, 0U))
  {
    PRINTF("f_lseek failed \r\n");
  }
  else
  {
    PRINTF("f_lseek success\r\n");
  }

  memset(g_data_read, 0U, sizeof(g_data_read));
  error = f_read(&g_fileObject, g_data_read, sizeof(g_data_read), &g_bytesRead);
  if ((error) || (g_bytesRead != sizeof(g_data_read)))
  {
    PRINTF("f_read failed\r\n");
  }
  else
  {
    PRINTF("f_read success\r\n");
  }

  if (memcmp(g_data_write, g_data_read, sizeof(g_data_write)))
  {
    PRINTF("memcmp failed\r\n");
  }
  else
  {
    PRINTF("memcmp success\r\n");
  }

  error = f_close(&g_fileObject);
  if (error)
  {
	  PRINTF("close file %s failed!\n", dir);
	  return error;
  }
  return error;
}
