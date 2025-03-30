/*
 * bsp_trng.c
 *
 *  Created on: 2025年3月30日
 *      Author: yongjie
 */
#include <stdint.h>
#include "bsp_trng.h"
#include "elog.h"

int test_trng(void)
{
	hal_rng_status_t status = HAL_RngInit();
	if (status != kStatus_HAL_RngSuccess)
	{
		log_e("HAL_RngInit failed:%x", status);
		return -1;
	}

//	uint32_t seed = 1024;
//	status = HAL_RngSetSeed(seed);
//	if (status != kStatus_HAL_RngSuccess)
//	{
//		log_e("HAL_RngSetSeed failed:%d", status);
//		return -1;
//	}

	uint32_t random = 0;

	status =  HAL_RngHwGetData((void *)&random, sizeof(random));
//	status =  HAL_RngGetData((void *)&random, sizeof(random));
	if (status != kStatus_HAL_RngSuccess)
	{
		log_e("HAL_RngHwGetData failed:%x", status);
		return -1;
	}

	log_i("random:%d", random);
	HAL_RngDeinit();
	return 0;
}
