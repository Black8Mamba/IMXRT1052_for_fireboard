/*
 * Copyright (c) 2018, NXP Semiconductors, Inc.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ffconf.h"
/* This fatfs subcomponent is disabled by default
 * To enable it, define following macro in ffconf.h */
#ifdef NOR_DISK_ENABLE

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "fsl_nor_disk.h"

/*******************************************************************************
 * Definitons
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*! @brief Card descriptor */

/*******************************************************************************
 * Code
 ******************************************************************************/
DRESULT nor_disk_write(uint8_t physicalDrive, const uint8_t *buffer, uint32_t sector, uint8_t count)
{
    assert(count == 1U);

    return RES_OK;
}

DRESULT nor_disk_read(uint8_t physicalDrive, uint8_t *buffer, uint32_t sector, uint8_t count)
{
    assert(count == 1U);


    return RES_OK;
}

DRESULT nor_disk_ioctl(uint8_t physicalDrive, uint8_t command, void *buffer)
{
    DRESULT result = RES_OK;

    if (physicalDrive != NORDISK)
    {
        return RES_PARERR;
    }

    return result;
}

DSTATUS nor_disk_status(uint8_t physicalDrive)
{
    if (physicalDrive != NORDISK)
    {
        return STA_NOINIT;
    }

    return 0;
}

DSTATUS nor_disk_initialize(uint8_t physicalDrive)
{

    if (physicalDrive != NORDISK)
    {
        return STA_NOINIT;
    }


    return 0;
}
#endif /* NOR_DISK_ENABLE */
