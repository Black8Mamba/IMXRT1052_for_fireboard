/*
 * aes.c
 *
 *  Created on: 2025年6月16日
 *      Author: yongjie
 */

#define LOG_TAG    "aes_test"
#define LOG_LVL    ELOG_LVL_DEBUG

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "mbedtls/cipher.h"
#include "mbedtls/platform.h"
#include "elog.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "mbedtls/cipher.h"
#include "mbedtls/platform.h"

/*
    # padding with pkcs7 AES_128_CBC Encrypt
    ptx = "CBC has been the most commonly used mode of operation."
    key = 06a9214036b8a15b512e03d534120006
    iv  = 3dafba429d9eb430b422da802c9fac41
    ctx = 4DDF9012D7B3898745A1ED9860EB0FA2
          FD2BBD80D27190D72A2F240C8F372A27
          63746296DDC2BFCE7C252B6CD7DD4BA8
          577E096DBD8024C8B4C5A1160CA2D3F9
*/
char *ptx = "CBC has been the most commonly used mode of operation.";
uint8_t key[] =
{
    0x06, 0xa9, 0x21, 0x40, 0x36, 0xb8, 0xa1, 0x5b,
    0x51, 0x2e, 0x03, 0xd5, 0x34, 0x12, 0x00, 0x06,
    0x06, 0xa9, 0x21, 0x40, 0x36, 0xb8, 0xa1, 0x5b,
    0x51, 0x2e, 0x03, 0xd5, 0x34, 0x12, 0x00, 0x06    
};

uint8_t iv[] =
{
    0x3d, 0xaf, 0xba, 0x42, 0x9d, 0x9e, 0xb4, 0x30,
    0xb4, 0x22, 0xda, 0x80, 0x2c, 0x9f, 0xac, 0x41
};

uint8_t ciphertext[256] = {0};
uint8_t plaintext[256] = {0};
int enc_len = 0;
int dec_len = 0;

static void dump_buf(char *info, uint8_t *buf, uint32_t len)
{
    for (int i = 0; i < len; i++) {
        elog_raw("%02x", buf[i]);
    }
    elog_raw("\r\n");
}

void enc(int type)
{
    size_t len;

    mbedtls_cipher_context_t ctx;
    const mbedtls_cipher_info_t *info;

    mbedtls_cipher_init(&ctx);
    info = mbedtls_cipher_info_from_type(type);

    if (info == NULL) {
        log_e("cipher info is NULL, type: %d\r\n", type);
        return;
    }

    if (mbedtls_cipher_setup(&ctx, info) != 0) {
        log_e("cipher setup failed, type: %d\r\n", type);
        return;
    }

    // log_i("cipher info setup, name: %s, block size: %d\r\n",
    //                     mbedtls_cipher_get_name(&ctx),
    //                     mbedtls_cipher_get_block_size(&ctx));

    mbedtls_cipher_setkey(&ctx, key, sizeof(key)*8, MBEDTLS_ENCRYPT);
    mbedtls_cipher_set_iv(&ctx, iv, sizeof(iv));
    mbedtls_cipher_set_padding_mode(&ctx, MBEDTLS_PADDING_PKCS7);
    // log_i("ptrx len: %d\r\n", strlen(ptx));
    mbedtls_cipher_update(&ctx, ptx, strlen(ptx), ciphertext, &len);
    enc_len += len;
    // log_i("cipher update, len: %d\r\n", len);
    // mbedtls_cipher_update(&ctx, ptx+strlen(ptx)-enc_len, strlen(ptx)-enc_len, ciphertext+enc_len, &len);
    // enc_len += len;
    // log_i("cipher update, len: %d\r\n", len);
    mbedtls_cipher_finish(&ctx, ciphertext + len, &len);
    // log_i("cipher finish, len: %d\r\n", len);
    enc_len += len;

//    dump_buf("cipher aes encrypt:", ciphertext, enc_len);

    mbedtls_cipher_free(&ctx);
    // log_i("encrypt done, enc_len: %d\r\n", enc_len);
}

void dec(int type)
{
	size_t len = 0;

    mbedtls_cipher_context_t ctx;
    const mbedtls_cipher_info_t *info;

    mbedtls_cipher_init(&ctx);
    info = mbedtls_cipher_info_from_type(type);

    mbedtls_cipher_setup(&ctx, info);
    // log_i("cipher info setup, name: %s, block size: %d\r\n",
    //                     mbedtls_cipher_get_name(&ctx),
    //                     mbedtls_cipher_get_block_size(&ctx));


    // 设置解密密钥
    mbedtls_cipher_setkey(&ctx, key, sizeof(key)*8, MBEDTLS_DECRYPT);
    mbedtls_cipher_set_iv(&ctx, iv, sizeof(iv));
    mbedtls_cipher_set_padding_mode(&ctx, MBEDTLS_PADDING_PKCS7);

    mbedtls_cipher_update(&ctx, ciphertext, enc_len, plaintext, &len);
    dec_len += len;
    // log_i("cipher update, len: %d\r\n", len);
    mbedtls_cipher_finish(&ctx, plaintext + dec_len, &len);
    // log_i("cipher finish, len: %d\r\n", len);
    mbedtls_cipher_free(&ctx);
    log_i("plaintext:%s\r\n", plaintext);

}


int test_aes(void)
{
	enc(MBEDTLS_CIPHER_AES_256_CBC);
	dec(MBEDTLS_CIPHER_AES_256_CBC);

    return 0;
}


