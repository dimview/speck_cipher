// Speck 128/128 with key schedule separate from encryption/decryption

#include <stdio.h>
#include <inttypes.h>

#define ROUNDS 32

static inline void speck_round(uint64_t& x, uint64_t& y, const uint64_t& k)
{
  x = (x >> 8) | (x << (8 * sizeof(x) - 8)); // x = ROTR(x, 8)
  x += y;
  x ^= k;
  y = (y << 3) | (y >> (8 * sizeof(y) - 3)); // y = ROTL(y, 3)
  y ^= x;
}

static inline void speck_back(uint64_t& x, uint64_t& y, const uint64_t& k)
{
  y ^= x;
  y = (y >> 3) | (y << (8 * sizeof(y) - 3)); // y = ROTR(y, 3)
  x ^= k;
  x -= y;
  x = (x << 8) | (x >> (8 * sizeof(x) - 8)); // x = ROTL(x, 8)
}

void speck_setup( const uint64_t key[2]
                , uint64_t key_schedule[ROUNDS]
                )
{
  uint64_t b = key[0];
  uint64_t a = key[1];
  key_schedule[0] = b; 
  for (unsigned i = 0; i < ROUNDS - 1; i++) {
    speck_round(a, b, i);
    key_schedule[i + 1] = b;
  }
}

void speck_encrypt( const uint64_t plaintext[2]
                  , const uint64_t key_schedule[ROUNDS]
                  , uint64_t ciphertext[2]
                  )
{
  ciphertext[0] = plaintext[0];
  ciphertext[1] = plaintext[1];
  for (unsigned i = 0; i < ROUNDS; i++) {
    speck_round(ciphertext[1], ciphertext[0], key_schedule[i]); 
  }
}
  
void speck_decrypt( const uint64_t ciphertext[2]
                  , const uint64_t key_schedule[ROUNDS]
                  , uint64_t decrypted[2]
                  )
{
  decrypted[0] = ciphertext[0];
  decrypted[1] = ciphertext[1];
  for (unsigned i = ROUNDS; i > 0; i--) {
    speck_back(decrypted[1], decrypted[0], key_schedule[i - 1]);
  }
}

int main(void)
{
  uint64_t plaintext[2] = {0x7469206564616d20ULL, 0x6c61766975716520ULL};
  uint64_t key[2] = {0x0706050403020100ULL, 0x0f0e0d0c0b0a0908ULL};
  uint64_t ciphertext[2];
  uint64_t decrypted[2];
  uint64_t key_schedule[ROUNDS];
  
  speck_setup(key, key_schedule);
  speck_encrypt(plaintext, key_schedule, ciphertext);
  speck_decrypt(ciphertext, key_schedule, decrypted);
  
  printf("Plaintext:  0x%016llx 0x%016llx\n", plaintext[0],  plaintext[1]);
  printf("Key:        0x%016llx 0x%016llx\n", key[0],        key[1]);
  printf("Ciphertext: 0x%016llx 0x%016llx\n", ciphertext[0], ciphertext[1]);
  printf("Expected:   0x7860fedf5c570d18 0xa65d985179783265\n");
  printf("Decrypted:  0x%016llx 0x%016llx\n", decrypted[0],  decrypted[1]);
  return 0;
}
