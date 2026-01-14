#include <string.h>
#include "twofish.h"
#include "key.h"

void twofish_encrypt_block(Twofish_ctx *ctx, uint8_t buf[16]) {
  uint32_t R[4];

  for (int i = 0; i < 4; i++) {
    // prepare a word
    R[i] = (
       ((uint32_t) buf[i * 4]) |
       ((uint32_t) buf[i * 4 + 1] << 8 ) |
       ((uint32_t) buf[i * 4 + 2] << 16) |
       ((uint32_t) buf[i * 4 + 3] << 24) );

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

    // modulo add round key
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

    buf[i * 4] = (uint8_t)((R[i]) & 0xFF);
    buf[i * 4 + 1] = (uint8_t)((R[i] >> 8) & 0xFF);
    buf[i * 4 + 2] = (uint8_t)((R[i] >> 16) & 0xFF);
    buf[i * 4 + 3] = (uint8_t)((R[i] >> 24) & 0xFF);
  }
}

void twofish_decrypt_block(Twofish_ctx *ctx, uint8_t buf[16]) {
  uint32_t R[4];

  for (int i = 0; i < 4; i++) {
     // prepare a word
     R[i] = (
       ((uint32_t) buf[i * 4]) |
       ((uint32_t) buf[i * 4 + 1] << 8 ) |
       ((uint32_t) buf[i * 4 + 2] << 16) |
       ((uint32_t) buf[i * 4 + 3] << 24) );

    // key whitening
    R[i] ^= ctx->exp_key[i+4];
  }

  // 16 reverse rounds
  for (int i = 15; i > -1; i--) {
    uint32_t temp0 = g_func(R[0], ctx->sbox);
    uint32_t temp1 = g_func(ROL32(R[1], 8), ctx->sbox);

    // PHT
    temp0 += temp1;
    temp1 += temp0;

    // modulo add round keys
    temp0 += ctx->exp_key[2 * i + 8];
    temp1 += ctx->exp_key[2 * i + 9];

    // undo the xor roation and apply rotations
    temp0 = ROL32(R[2], 1) ^ temp0;
    temp1 = ROR32(R[3] ^ temp1, 1);

    // swap
    if (i > 0) {
      R[2] = R[0];
      R[3] = R[1];
      R[0] = temp0;
      R[1] = temp1;
    } else {
      R[2] = temp0;
      R[3] = temp1;
    }
  }

  // key whitening
  for (int i = 0; i < 4; i++) {
    R[i] ^= ctx->exp_key[i];

    buf[i * 4]     = (uint8_t)(R[i] & 0xFF);
    buf[i * 4 + 1] = (uint8_t)((R[i] >> 8) & 0xFF);
    buf[i * 4 + 2] = (uint8_t)((R[i] >> 16) & 0xFF);
    buf[i * 4 + 3] = (uint8_t)((R[i] >> 24) & 0xFF);
  }
}

void twofish_init(Twofish_ctx *ctx, const uint8_t key[], int key_len) {
  ctx->k = key_len / 64;
  key_schedule(ctx, key);
}

// CBC mode for general encryption
void twofish_encrypt_cbc(Twofish_ctx *ctx, uint8_t buf[], const int plain_len, int *cipher_len) {
  uint8_t prev_block[16];
  int i, j;
  int full_blocks = plain_len / 16;
  int remaining = plain_len % 16;
  int pad = 16 - remaining;

  // encrypt full blocks
  memcpy(prev_block, ctx->iv, 16);
  for (i = 0; i < full_blocks; i++) {
    for (j = 0; j < 16; j++) {
      buf[i * 16 + j] = prev_block[j] ^ buf[i * 16 + j];
    }
    twofish_encrypt_block(ctx, &buf[i * 16]);
    memcpy(prev_block, &buf[i * 16], 16);
  }

  // xor remaining bits
  for (j = 0; j < remaining; j++) {
    buf[i * 16 + j] = prev_block[j] ^ buf[i * 16 + j];
  }

  // add padding
  for (j = remaining; j < 16; j++) {
    buf[i * 16 + j] = prev_block[j] ^ pad;
  }
  // encrypt final block
  twofish_encrypt_block(ctx, &buf[i * 16]);
  *cipher_len = (i + 1) * 16;

  // modify iv in case of need of encrypting another buffer
  memcpy(ctx->iv, &buf[i * 16], 16);
}

// CBC mode for general decryption
void twofish_decrypt_cbc(Twofish_ctx *ctx, uint8_t buf[], const int cipher_len, int *plain_len) {
  int i, j;
  // int cipher_blocks = cipher_len / 16;
  uint8_t prev_block[16];

  // decryption
  for (i = 0; i < cipher_len; i += 16) {
    memcpy(prev_block, &buf[i], 16);
    twofish_decrypt_block(ctx, &buf[i]);
    for (j = 0; j < 16; j++) {
      buf[i + j] ^= ctx->iv[j];
    }
    memcpy(ctx->iv, prev_block, 16);
  }

  uint8_t pad = buf[cipher_len - 1];

  if (pad > 0 && pad <= 16) {
    *plain_len = cipher_len - pad;
  } else {
    *plain_len = cipher_len; // invalid padding
  }
}
