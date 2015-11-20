// Speck 64/128 wrapper for use in TestU01

#include "speck_64_128.h"
#include "unif01.h"
#include "util.h"
#include "addstr.h"
#include <string.h>
#include <inttypes.h>
#include <math.h>

#define ROUNDS 27
#define ROR8(x) ((x >> 8) | (x << (32 - 8)))
#define ROL3(y) ((y << 3) | (y >> (32 - 3)))
#define SPECK_ROUND(x, y, k) { x = (ROR8(x) + y) ^ k; y = ROL3(y) ^ x; }

void speck_setup( const uint32_t key[4]
                , uint32_t key_schedule[ROUNDS]
                )
{
  uint32_t b = key[0];
  uint32_t a[3];
  a[0] = key[1];
  a[1] = key[2];
  a[2] = key[3];
  key_schedule[0] = b;  
  for (unsigned i = 0; i < ROUNDS - 1; i++) {
    SPECK_ROUND(a[i % 3], b, i);
    key_schedule[i + 1] = b;
  }
}

void speck_encrypt( const uint32_t plaintext[2]
                  , const uint32_t key_schedule[ROUNDS]
                  , uint32_t ciphertext[2]
                  )
{
  ciphertext[0] = plaintext[0];
  ciphertext[1] = plaintext[1];
  for (unsigned i = 0; i < ROUNDS; i++) {
    SPECK_ROUND(ciphertext[1], ciphertext[0], key_schedule[i]); 
  }
}

typedef struct { 
  uint32_t key_schedule[ROUNDS]; 
  uint32_t counter[2];
} Speck64_128_state;

static double Speck64_128_U01 (void *par, void *sta)
{
   Speck64_128_state *state = sta;
   if (++state->counter[0] == 0) ++state->counter[1];
   uint32_t ciphertext[2];
   speck_encrypt(state->counter, state->key_schedule, ciphertext);
   return ldexp(ciphertext[0] * (double)(1ULL << sizeof(ciphertext[0]) * 8) + ciphertext[1], -64);
}

static unsigned long Speck64_128_Bits (void *par, void *sta)
{
   Speck64_128_state *state = sta;
   uint32_t ciphertext[2];
   if (++state->counter[0] == 0) ++state->counter[1];
   speck_encrypt(state->counter, state->key_schedule, ciphertext);
   return ciphertext[0];
}

static void WrSpeck64_128 (void *sta)
{
  uint32_t plaintext[2] = {0x7475432dUL, 0x3b726574UL};
  uint32_t key[4] = {0x03020100UL, 0x0b0a0908UL, 0x13121110UL, 0x1b1a1918UL};
  uint32_t ciphertext[2];
  uint32_t key_schedule[ROUNDS];

  speck_setup(key, key_schedule);
  speck_encrypt(plaintext, key_schedule, ciphertext);
  if (ciphertext[0] == 0x454e028b && ciphertext[1] == 0x8c6fa548) {
    printf(" Self-check passed\n");
  } else {
    printf(" Self-check failed\n");
    printf(" Plaintext:  0x%08x 0x%08x\n", plaintext[0],  plaintext[1]);
    printf(" Key:        0x%08x 0x%08x 0x%08x 0x%08x\n", key[0], key[1], key[2], key[3]);
    printf(" Ciphertext: 0x%08x 0x%08x\n", ciphertext[0], ciphertext[1]);
    printf(" Expected:   0x454e028b 0x8c6fa548\n");
  }

   Speck64_128_state *state = sta;
   printf (" Counter = 0x%08x 0x%08x\n", state->counter[0], state->counter[1]);
}

unif01_Gen *CreateSpeck64_128 (int s)
{
   unif01_Gen *gen;
   Speck64_128_state *state;
   size_t leng;
   char name[60];

   gen = util_Malloc (sizeof (unif01_Gen));
   gen->state = state = util_Malloc (sizeof (Speck64_128_state));
   state->counter[0] = s;
   state->counter[1] = 0;
   uint32_t key[4] = {0x03020100UL, 0x0b0a0908UL, 0x13121110UL, 0x1b1a1918UL}; // Can be random
   speck_setup(key, state->key_schedule);

   gen->param = NULL;
   gen->Write = WrSpeck64_128;
   gen->GetU01 = Speck64_128_U01;
   gen->GetBits = Speck64_128_Bits;

   strcpy (name, "Speck 64/128:");
   addstr_Int (name, "   s = ", s);
   leng = strlen (name);
   gen->name = util_Calloc (leng + 1, sizeof (char));
   strncpy (gen->name, name, leng);
   return gen;
}

void DeleteSpeck64_128 (unif01_Gen * gen)
{
   gen->state = util_Free (gen->state);
   gen->name = util_Free (gen->name);
   util_Free (gen);
}
