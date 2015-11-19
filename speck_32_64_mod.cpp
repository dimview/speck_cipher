// Speck 32/64 using ring buffer

#include <stdio.h>
#include <inttypes.h>

#define ROUNDS 22

static inline void speck_round(uint16_t& x, uint16_t& y, const uint16_t k)
{
  x = (x >> 7) | (x << (16 - 7)); // x = ROTR(x, 7)
  x += y;
  x ^= k;
  y = (y << 2) | (y >> (16 - 2)); // y = ROTL(y, 2)
  y ^= x;
}

void speck_block( const uint16_t plaintext[2]
                , const uint16_t key[4]
                , uint16_t ciphertext[2]
                )
{
  ciphertext[0] = plaintext[0];
  ciphertext[1] = plaintext[1];
  uint16_t b  = key[0];
  uint16_t a[3];
  a[0] = key[1]; // ring buffer
  a[1] = key[2];
  a[2] = key[3];
  for (unsigned i = 0; i < ROUNDS; i++) {
    speck_round(ciphertext[1], ciphertext[0], b);
    speck_round(a[i % 3], b, i);
  }
}

int main(void)
{
  uint16_t plaintext[2] = {0x694c, 0x6574};
  uint16_t key[4] = {0x0100, 0x0908, 0x1110, 0x1918};
  uint16_t ciphertext[2];
  speck_block(plaintext, key, ciphertext);
  printf("Plaintext:  0x%04x 0x%04x\n", plaintext[0], plaintext[1]);
  printf("Key:        0x%04x 0x%04x 0x%04x 0x%04x\n", key[0], key[1], key[2], key[3]);
  printf("Ciphertext: 0x%04x 0x%04x\n", ciphertext[0], ciphertext[1]);
  printf("Expected:   0x42f2 0xa868\n");
  return 0;
}
