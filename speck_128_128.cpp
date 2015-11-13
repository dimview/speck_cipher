// Speck 128/128

#include <stdio.h>
#include <inttypes.h>

static inline void speck_round(uint64_t& x, uint64_t& y, uint64_t k)
{
  x = (x >> 8) | (x << (64 - 8)); // x = ROTR(x, 8)
  x += y;
  x ^= k;
  y = (y << 3) | (y >> (64 - 3)); // y = ROTL(y, 3)
  y ^= x;
}

void speck_block( const uint64_t plaintext[2]
                , const uint64_t key[2]
                , uint64_t ciphertext[2]
                )
{
  uint64_t b = key[0];
  uint64_t a = key[1];
  ciphertext[0] = plaintext[0];
  ciphertext[1] = plaintext[1];
  for (uint64_t i = 0; i < 32; i++) {
    speck_round(ciphertext[1], ciphertext[0], b); 
    speck_round(a, b, i);
  }
}
  
int main(void)
{
  uint64_t plaintext[2] = {0x7469206564616d20ULL, 0x6c61766975716520ULL};
  uint64_t key[2] = {0x0706050403020100ULL, 0x0f0e0d0c0b0a0908Ull};

  uint64_t ciphertext[2];
  speck_block(plaintext, key, ciphertext);
  printf("Plaintext:  0x%016llx 0x%016llx\n", plaintext[0],  plaintext[1]);
  printf("Key:        0x%016llx 0x%016llx\n", key[0],        key[1]);
  printf("Ciphertext: 0x%016llx 0x%016llx\n", ciphertext[0], ciphertext[1]);
  printf("Expected:   0x7860fedf5c570d18 0xa65d985179783265\n");
  return 0;
}
