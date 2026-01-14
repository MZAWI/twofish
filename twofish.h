#pragma once
#include <stdint.h>
#include "math.h"
#include "key.h"

void twofish_encrypt_block(Twofish_ctx *ctx, uint8_t buf[16]);
void twofish_decrypt_block(Twofish_ctx *ctx, uint8_t buf[16]);

void twofish_encrypt_cbc(Twofish_ctx *ctx, uint8_t buf[], const int plain_len, int *cipher_len);
void twofish_decrypt_cbc(Twofish_ctx *ctx, uint8_t buf[], const int cipher_len, int *plain_len);
