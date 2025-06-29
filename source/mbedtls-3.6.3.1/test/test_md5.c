/*
 * test_md5.c
 *
 *  Created on: 2025年6月29日
 *      Author: yongjie
 */
#define LOG_TAG    "md5_test"
#define LOG_LVL    ELOG_LVL_DEBUG
#include "mbedtls/md5.h"
#include <string.h>

int test_md5(void)
{
    const char *input = "Hello, mbedtls MD5!";
    unsigned char md5[16];

    // 计算MD5
    mbedtls_md5_context ctx;
    mbedtls_md5_init(&ctx);
    mbedtls_md5_starts(&ctx);
    mbedtls_md5_update(&ctx, (const unsigned char *)input, strlen(input));
    mbedtls_md5_finish(&ctx, md5);
    mbedtls_md5_free(&ctx);

    log_i("Input: %s\r\n", input);

    return 0;
}
