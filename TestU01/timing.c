// Compare timing of Speck 64/128 and PCG32
//
// To build:
// gcc -Wall -O3 speck_64_128.c pcg-random.c timing.c /usr/local/lib/libtestu01.a /usr/local/lib/libmylib.a /usr/local/lib/libprobdist.a -o timing

#include "speck_64_128.h"
#include "pcg-random.h"
#include "unif01.h"
#include "bbattery.h"

int main(void)
{
  unif01_Gen * gen;

  gen = CreateSpeck64_128(42);
  unif01_TimerSumGenWr(gen, 1000000000, TRUE);
  DeleteSpeck64_128(gen);

  gen = CreatePCG32(42);
  unif01_TimerSumGenWr(gen, 1000000000, TRUE);
  DeletePCG32(gen);

  return 0;
}