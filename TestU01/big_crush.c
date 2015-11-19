// Run Speck 64/128 through TestU01
//
// To build:
// gcc -Wall -O3 speck_64_128.c big_crush.c /usr/local/lib/libtestu01.a /usr/local/lib/libmylib.a /usr/local/lib/libprobdist.a -o big_crush
//
#include "speck_64_128.h"
#include "unif01.h"
#include "bbattery.h"

int main(void)
{
  unif01_Gen * gen;
  gen = CreateSpeck64_128(42);
  bbattery_BigCrush(gen);
  DeleteSpeck64_128(gen);
  return 0;
}