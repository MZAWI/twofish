#pragma once
#include <stdint.h>
#include "math.h"
#include "key.h"

void twofish_encrypt_block(Twofish_ctx *ctx, const uint8_t plain[16], uint8_t cipher[16]);
