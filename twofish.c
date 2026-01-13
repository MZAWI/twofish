#include "twofish.h"
#include "key.h"

void twofish_encrypt_block(Twofish_ctx *ctx, const uint8_t plain[16], uint8_t cipher[16]) {
  uint32_t R[4];

  for (int i = 0; i < 4; i++) {
    // prepare a word
    R[i] = (
       ((uint32_t) plain[i*4]) |
       ((uint32_t) plain[i*4 + 1] << 8 ) |
       ((uint32_t) plain[i*4 + 2] << 16) |
       ((uint32_t) plain[i*4 + 3] << 24) );

    // key whitening
    R[i] ^= ctx->exp_key[i];
  }
  // 16 rounds
  for (int i = 0; i < 16; i++) {
    uint32_t temp0 = g_func(R[0], ctx->sbox);
    uint32_t temp1 = g_func(ROL32(R[1], 8), ctx->sbox);

    // PHT
    temp0 += temp1;
    temp1 += temp0;

    // modulo add key
    temp0 += ctx->exp_key[2 * i + 8];
    temp1 += ctx->exp_key[2 * i + 9];

    // xor with right side
    temp0 = ROR32(temp0 ^ R[2], 1);
    temp1 = temp1 ^ ROL32(R[3], 1);

    if (i < 15) {
    // swap sides
    R[2] = R[0];
    R[3] = R[1];
    R[0] = temp0;
    R[1] = temp1;
    } else {
      R[2] = temp0;
      R[3] = temp1;
    }
  }
  
  for (int i = 0; i < 4; i++) {
    // key whitening
    R[i] ^= ctx->exp_key[i+4];

    cipher[i * 4] = (uint8_t)((R[i]) & 0xFF);
    cipher[i * 4 + 1] = (uint8_t)((R[i] >> 8) & 0xFF);
    cipher[i * 4 + 2] = (uint8_t)((R[i] >> 16) & 0xFF);
    cipher[i * 4 + 3] = (uint8_t)((R[i] >> 24) & 0xFF);
  }
}



