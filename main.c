#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "crapto1.h"

#define TEST_ROUND 100

int main(int argc, char const* argv[]) {
  uint64_t lfsr, sum, test, ks_sum;
  uint64_t ks3[16] = {0};
  uint32_t num;
  uint32_t nt;
  uint8_t k;

  srand(time(NULL));

  assert(parity(0b00000000) == 0);
  assert(parity(0b00000001) == 1);
  assert(parity(0b11111111) == 0);
  assert(parity(0b11111110) == 1);

  nt = rand() % 65536;

  sum = ks_sum = 0;
  for (test = 0; test < TEST_ROUND; test++) {
    uint64_t key = 0;
    key = rand() % 65536 & 0xffff;
    key = key << 16 | rand() % 65536 & 0xffff;
    key = key << 16 | rand() % 65536 & 0xffff;
    // printf("key   : %012" PRIx64 "\n", key);
    for (num = 0; num < 65536; num++) {
      struct Crypto1State* state = crypto1_create(key);
      uint8_t i, j, p, ks;

      nt = prng_successor(nt, rand() % 256);
      // printf("nT  : %x\n", nt);

      crypto1_word(state, nt, 0);

      // crypto1_get_lfsr(state, &lfsr);
      // printf("a32 : %" PRIx64 "\n", lfsr);

      p = 0;
      for (j = 0; j < 4; j++) {
        ks = crypto1_byte(state, 0, 1);  // nR(j) => ks1(j) , j = 0 ~ 3
        p = p + (parity(ks) ^ filter(state->odd));
      }

      if (p != 4) {
        continue;
      }

      p = 0;
      for (j = 0; j < 4; j++) {
        ks = crypto1_byte(state, 0, 0);  // aR(j) => ks2(j) , j = 0 ~ 3
        p = p + (parity(ks) ^ filter(state->odd));
      }

      if (p != 4) {
        continue;
      }

      ks = 0;
      for (i = 0; i < 4; i++) {
        ks |= crypto1_bit(state, 0, 0) << i;
      }

      crypto1_destroy(state);

      ks3[ks] += 1;
      ks_sum++;
      if (ks == 0b0000) {
        // printf("nT num: %5d\n", num);
        num++;
        break;
      }
    }

    sum += num;
  }

  printf("%5" PRIu64 " tests, %0.2f\n", test, sum * 1.0 / test);

  for (k = 0; k < 16; k++) {
    printf("ks[%d]: %0.3f\n", k, ks3[k] * 100.0 / ks_sum);
    ks3[k] = 0;
  }

  sum = ks_sum = 0;
  for (test = 0; test < TEST_ROUND; test++) {
    uint64_t key = 0;
    key = rand() % 65536 & 0xffff;
    key = key << 16 | rand() % 65536 & 0xffff;
    key = key << 16 | rand() % 65536 & 0xffff;
    // printf("key   : %012" PRIx64 "\n", key);
    for (num = 0; num < 65536; num++) {
      struct Crypto1State* state = crypto1_create(key);
      uint8_t i, j, p, ks;

      nt = prng_successor(nt, rand() % 256);
      // printf("nT  : %x\n", nt);

      crypto1_word(state, nt, 0);

      // crypto1_get_lfsr(state, &lfsr);
      // printf("a32 : %" PRIx64 "\n", lfsr);

      p = 0;
      for (j = 0; j < 4; j++) {
        ks = crypto1_byte(state, 0xff, 1);  // nR(j) => ks1(j) , j = 0 ~ 3
        p = p + (parity(ks) ^ filter(state->odd));
      }

      if (p > 0) {
        continue;
      }

      p = 0;
      for (j = 0; j < 4; j++) {
        ks = crypto1_byte(state, 0, 0);  // aR(j) => ks2(j) , j = 0 ~ 3
        p = p + (parity(ks) ^ filter(state->odd));
      }

      if (p > 0) {
        continue;
      }

      ks = 0;
      for (i = 0; i < 4; i++) {
        ks |= crypto1_bit(state, 0, 0) << i;
      }

      crypto1_destroy(state);

      ks3[ks] += 1;
      ks_sum++;
      if (ks == 0b1111) {
        // printf("nT num: %5d\n", num);
        num++;
        break;
      }
    }

    sum += num;
  }

  printf("%5" PRIu64 " tests, %0.2f\n", test, sum * 1.0 / test);

  for (k = 0; k < 16; k++) {
    printf("ks[%d]: %0.3f\n", k, ks3[k] * 100.0 / ks_sum);
  }

  return 0;
}
