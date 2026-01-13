#pragma once
#include <stdint.h>

// x^8 = x^6 + x^3 + x^2 + 1
#define RS_POLYNOMIAL 0x4D // reduction polynomial for RS multiplication
// x^8 = x^6 + x^5 + x^3 + 1
#define MDS_POLYNOMIAL 0x69 // reduction polynomial for MDS multiplication

// left/right 32-bit rotations
#define ROL32(x, n) (((uint32_t)(x) << (n)) | ((uint32_t)(x) >> (32 - (n))))
#define ROR32(x, n) (((uint32_t)(x) >> (n)) | ((uint32_t)(x) << (32 - (n))))

// left/right 4-bit rotations
#define ROL4(x, n) ((((uint8_t)(x) << (n)) | ((uint8_t)(x) >> (4 - (n)))) & 0x0F)
#define ROR4(x, n) ((((uint8_t)(x) >> (n)) | ((uint8_t)(x) << (4 - (n)))) & 0x0F)

#define q0 0
#define q1 1

// multiplication over galois field
uint8_t gf_mult(uint8_t a, uint8_t b, const uint8_t polynomial);

// h function - k = 2/3/4 as defined in Twofish paper
uint32_t h_func(uint32_t X, const uint32_t L[], int k);
uint32_t h_lane(uint8_t x_i, int lane, const uint32_t L[], int k);
uint32_t g_func(uint32_t X, uint32_t sbox[4][256]);

uint8_t q_perm(uint8_t x, int type);
