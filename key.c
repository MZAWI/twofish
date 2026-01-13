#include "key.h"
#include "math.h"
#include <stdint.h>

// Reed-Solomon matrix used only in the key schedule
static const uint8_t RS[4][8] = {
    {0x01, 0xA4, 0x55, 0x87, 0x5A, 0x58, 0xDB, 0x9E},
    {0xA4, 0x56, 0x82, 0xF3, 0x1E, 0xC6, 0x68, 0xE5},
    {0x02, 0xA1, 0xFC, 0xC1, 0x47, 0xAE, 0x3D, 0x19},
    {0xA4, 0x55, 0x87, 0x5A, 0x58, 0xDB, 0x9E, 0x03}};

// produce a single word for a third key vector S
// expects a pointer to an 8-byte key part
uint32_t rs_mul_word(const uint8_t key_group[8]) {
  uint8_t s_word[4] = {0};
  // each RS row
  for (int i = 0; i < 4; i++) {
    // row multiplication
    for (int j = 0; j < 8; j++) {
      s_word[i] ^= gf_mult(RS[i][j], key_group[j], RS_POLYNOMIAL);
    }
  }
  // return 32-bit word
  return (((uint32_t)s_word[0]) | ((uint32_t)s_word[1] << 8) |
          ((uint32_t)s_word[2] << 16) | ((uint32_t)s_word[3] << 24));
}

// produce expanded key material
void key_schedule(Twofish_ctx *ctx, uint8_t key[]) {
  const int k = ctx->key_len / 64;
  const uint32_t p = 0x01010101;

  // first key conversion
  uint32_t M[8];
  for (int i = 0; i < 2 * k; i++) {
      M[i] = (
      ((uint32_t) key[4 * i]) |
      ((uint32_t) key[4 * i + 1] << 8 ) |
      ((uint32_t) key[4 * i + 2] << 16 ) |
      ((uint32_t) key[4 * i + 3] << 24 )
    );
  }

  // two word vectors M_e and M_o of length k
  uint32_t M_e[4];
  uint32_t M_o[4];

  for (int i = 0; i < k; i++) {
    M_e[i] = M[2 * i];
    M_o[i] = M[2 * i + 1];
  }

  // third word vector S of length k
  // Note that S lists the words in "reverse" order.
  uint32_t S[k];
  for (int i = 0; i < k; i++) {
    S[k - i - 1] = rs_mul_word(&key[i * 8]);
  }

  // precompute S-boxes for the g function
  for (uint32_t i = 0; i < 256; i++) {
    ctx->sbox[0][i] = h_lane(i, 0, S, k);
    ctx->sbox[1][i] = h_lane(i, 1,  S, k);
    ctx->sbox[2][i] = h_lane(i, 2, S, k);
    ctx->sbox[3][i] = h_lane(i, 3, S, k);
  }

  // the expanded key words
  for (int i = 0; i < 20; i++) {
    uint32_t A = h_func(2 * i * p, M_e, k);
    uint32_t B = h_func((2 * i + 1) * p, M_o, k);

    B = ROL32(B, 8);

    ctx->exp_key[2 * i] = A + B;
    ctx->exp_key[2 * i + 1] = ROL32((A + 2 * B), 9);
  }
}
