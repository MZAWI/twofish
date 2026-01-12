#pragma once
#include <stdint.h>

typedef struct {
  uint32_t sbox[4][256];
  uint32_t exp_key[40];
  int key_len;
} Twofish_ctx;

void key_schedule(Twofish_ctx *ctx, uint8_t key[]);
uint32_t rs_mul_word(const uint8_t key_group[8]);


