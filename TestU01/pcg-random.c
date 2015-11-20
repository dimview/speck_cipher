// PCG32 wrapper for use in TestU01

#include "pcg-random.h"
#include "unif01.h"
#include "util.h"
#include "addstr.h"
#include <string.h>
#include <inttypes.h>
#include <math.h>

// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)

typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

uint32_t pcg32_random_r(pcg32_random_t* rng)
{
    uint64_t oldstate = rng->state;
    // Advance internal state
    rng->state = oldstate * 6364136223846793005ULL + (rng->inc|1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

static unsigned long pcg32_Bits (void *par, void *sta)
{
  return (uint64_t)pcg32_random_r((pcg32_random_t *)sta) << 32 
                 | pcg32_random_r((pcg32_random_t *)sta);
}

static double pcg32_U01 (void *par, void *sta)
{
   return ldexp(pcg32_Bits(par, sta), -64);
}

static void WrPCG32(void *sta)
{
}

unif01_Gen *CreatePCG32 (int s)
{
   unif01_Gen *gen;
   pcg32_random_t *state;
   size_t leng;
   char name[60];

   gen = util_Malloc (sizeof (unif01_Gen));
   gen->state = state = util_Malloc (sizeof (pcg32_random_t));
   state->state = 0;
   state->inc = s;
   gen->param = NULL;
   gen->Write = WrPCG32;
   gen->GetU01 = pcg32_U01;
   gen->GetBits = pcg32_Bits;

   strcpy (name, "PCG32:");
   addstr_Int (name, "   s = ", s);
   leng = strlen (name);
   gen->name = util_Calloc (leng + 1, sizeof (char));
   strncpy (gen->name, name, leng);
   return gen;
}

void DeletePCG32 (unif01_Gen * gen)
{
   gen->state = util_Free (gen->state);
   gen->name = util_Free (gen->name);
   util_Free (gen);
}
