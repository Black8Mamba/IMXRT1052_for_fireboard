#define LOG_TAG    "hash_test"
#define LOG_LVL    ELOG_LVL_DEBUG

#include "mbedtls/sha256.h"
#include <stdio.h>
#include <string.h>
#include "elog.h"

int test_hash(void)
{
    const char *input = "Hello, mbedtls SHA256!";
    unsigned char hash[32];
    char output[65];

    // 计算SHA256
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0); // 0: SHA-256, 1: SHA-224
    mbedtls_sha256_update(&ctx, (const unsigned char *)input, strlen(input)); // 可分段流式计算
    mbedtls_sha256_finish(&ctx, hash);
    mbedtls_sha256_free(&ctx);

    // 转为十六进制字符串
    for (int i = 0; i < 32; i++) {
        sprintf(output + i * 2, "%02x", hash[i]);
    }
    output[64] = '\0';

    log_i("Input: %s\r\n", input);
    log_i("SHA256: %s\r\n", output);

    return 0;
}
