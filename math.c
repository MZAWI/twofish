#include <stdint.h>
#include "math.h"

static const uint8_t MDS[4][4] = {{0x01, 0xEF, 0x5B, 0x5B},
                                  {0x5B, 0xEF, 0xEF, 0x01},
                                  {0xEF, 0x5B, 0x01, 0xEF},
                                  {0xEF, 0x01, 0xEF, 0x5B}};

// t lookup tables used in q_0 and q_1 permutations
// first is q_0 second is q_1
static const uint8_t t[2][4][16] = {
{
    {0x8, 0x1, 0x7, 0xD, 0x6, 0xF, 0x3, 0x2, 0x0, 0xB, 0x5, 0x9, 0xE, 0xC, 0xA, 0x4},
    {0xE, 0xC, 0xB, 0x8, 0x1, 0x2, 0x3, 0x5, 0xF, 0x4, 0xA, 0x6, 0x7, 0x0, 0x9, 0xD},
    {0xB, 0xA, 0x5, 0xE, 0x6, 0xD, 0x9, 0x0, 0xC, 0x8, 0xF, 0x3, 0x2, 0x4, 0x7, 0x1},
    {0xD, 0x7, 0xF, 0x4, 0x1, 0x2, 0x6, 0xE, 0x9, 0xB, 0x3, 0x0, 0x8, 0x5, 0xC, 0xA}
},
{
    {0x2, 0x8, 0xB, 0xD, 0xF, 0x7, 0x6, 0xE, 0x3, 0x1, 0x9, 0x4, 0x0, 0xA, 0xC, 0x5},
    {0x1, 0xE, 0x2, 0xB, 0x4, 0xC, 0x3, 0x7, 0x6, 0xD, 0xA, 0x5, 0xF, 0x9, 0x0, 0x8},
    {0x4, 0xC, 0x7, 0x5, 0x1, 0x6, 0x9, 0xA, 0x0, 0xE, 0xD, 0x8, 0x2, 0xB, 0x3, 0xF},
    {0xB, 0x9, 0x5, 0x1, 0xC, 0x3, 0xD, 0xE, 0x6, 0x4, 0x7, 0xF, 0x2, 0x0, 0x8, 0xA}
} 
};

// q_0/q_1 permutations - specify which with q0/q1 macros as type variable
uint8_t q_perm(uint8_t x, int type) {
  uint8_t a_0 = x >> 4;
  uint8_t b_0 = x & 0x0F;

  uint8_t a_1 = a_0 ^ b_0;
  uint8_t b_1 = (a_0 ^ ROR4(b_0, 1) ^ (8 * a_0)) & 0x0F;

  uint8_t a_2 = t[type][0][a_1];
  uint8_t b_2 = t[type][1][b_1];

  uint8_t a_3 = a_2 ^ b_2;
  uint8_t b_3 = (a_2 ^ ROR4(b_2, 1) ^ (8 * a_2)) & 0x0F;

  uint8_t a_4 = t[type][2][a_3];
  uint8_t b_4 = t[type][3][b_3];

  return (b_4 << 4) | a_4;
}

uint8_t gf_mult(uint8_t a, uint8_t b, const uint8_t polynomial) {
  uint8_t result = 0;
  for (int i = 0; i < 8; i++) {
    // multiply multiplicant a and shift for next iteration
    if (a & 1)
      result ^= b;
    a >>= 1;
    // shift multiplier, apply polynomial reduction if overflows
    if (b >> 7) {
      b <<= 1;
      b ^= polynomial;
    } else
      b <<= 1;
  }
  return result;
}


uint32_t h_func(uint32_t X, const uint32_t L[], int k) {
  uint8_t l[4][4];
  for (int i = 0; i < k; i++) {
    l[i][0] = (uint8_t)(L[i] & 0xFF);
    l[i][1] = (uint8_t)((L[i] >> 8) & 0xFF);
    l[i][2] = (uint8_t)((L[i] >> 16) & 0xFF);
    l[i][3] = (uint8_t)((L[i] >> 24) & 0xFF);
  }

  uint8_t x[4];
  x[0] = (uint8_t) (X & 0xFF);
  x[1] = (uint8_t) ((X >> 8) & 0xFF);
  x[2] = (uint8_t) ((X >> 16) & 0xFF);
  x[3] = (uint8_t) ((X >> 24) & 0xFF);

  if (k == 4) {
    x[0] = q_perm(x[0], q1) ^ l[3][0];
    x[1] = q_perm(x[1], q0) ^ l[3][1];
    x[2] = q_perm(x[2], q0) ^ l[3][2];
    x[3] = q_perm(x[3], q1) ^ l[3][3];
  }
  if (k >= 3) {
    x[0] = q_perm(x[0], q1) ^ l[2][0];
    x[1] = q_perm(x[1], q1) ^ l[2][1];
    x[2] = q_perm(x[2], q0) ^ l[2][2];
    x[3] = q_perm(x[3], q0) ^ l[2][3];
  }

  x[0] = q_perm(q_perm(q_perm(x[0], q0) ^ l[1][0], q0) ^ l[0][0], q1);
  x[1] = q_perm(q_perm(q_perm(x[1], q1) ^ l[1][1], q0) ^ l[0][1], q0);
  x[2] = q_perm(q_perm(q_perm(x[2], q0) ^ l[1][2], q1) ^ l[0][2], q1);
  x[3] = q_perm(q_perm(q_perm(x[3], q1) ^ l[1][3], q1) ^ l[0][3], q0);

  // MDS matrix multiplication
  uint8_t Z[4] = {0};
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      Z[i] ^= gf_mult(MDS[i][j], x[j], MDS_POLYNOMIAL);
    }
  }

  return (
  ((uint32_t) Z[0])       |
  ((uint32_t) Z[1] << 8)  |
  ((uint32_t) Z[2] << 16) |
  ((uint32_t) Z[3] << 24) );
}








