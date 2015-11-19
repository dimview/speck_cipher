// Speck 64/128

#include <stdio.h>
#include <inttypes.h>

#define ROUNDS 27

static inline void speck_round(uint32_t& x, uint32_t& y, uint32_t k)
{
  x = (x >> 8) | (x << (8 * sizeof(x) - 8)); // x = ROTR(x, 8)
  x += y;
  x ^= k;
  y = (y << 3) | (y >> (8 * sizeof(y) - 3)); // y = ROTL(y, 3)
  y ^= x;
}

void speck_encrypt( const uint32_t plaintext[2]
                  , const uint32_t key[4]
                  , uint32_t ciphertext[2]
                  )
{
  uint32_t b = key[0];
  uint32_t a[3];
  a[0] = key[1];
  a[1] = key[2];
  a[2] = key[3];
  ciphertext[0] = plaintext[0];
  ciphertext[1] = plaintext[1];
  for (unsigned i = 0; i < ROUNDS; i++) {
    speck_round(ciphertext[1], ciphertext[0], b); 
    speck_round(a[i % 3], b, i);
  }
}
  
int main(void)
{
  uint32_t plaintext[2] = {0x7475432dUL, 0x3b726574UL};
  uint32_t key[4] = {0x03020100UL, 0x0b0a0908UL, 0x13121110UL, 0x1b1a1918UL};

  uint32_t ciphertext[2];
  speck_encrypt(plaintext, key, ciphertext);
  printf("Plaintext:  0x%08x 0x%08x\n", plaintext[0],  plaintext[1]);
  printf("Key:        0x%08x 0x%08x 0x%08x 0x%08x\n", key[0], key[1], key[2], key[3]);
  printf("Ciphertext: 0x%08x 0x%08x\n", ciphertext[0], ciphertext[1]);
  printf("Expected:   0x454e028b 0x8c6fa548\n");
  return 0;
}
