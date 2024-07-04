/* kompilacja: gcc @opcje przyklad.c ary.c -o przyklad.e -lm */

#include <stdio.h>
#include <math.h>

#include "ary.h"

int main() {
  wartosc jeden = wartosc_dokladna(1);
  wartosc zero = wartosc_dokladna(0);

  printf("1=%d\n", in_wartosc(razy(jeden, zero), 0));
  
  printf("1=%d\n", in_wartosc(razy(zero, wartosc_od_do(1., 10.)), 0));
  
  wartosc malo = wartosc_od_do(0., 1.);
  
  printf("1=%d\n", in_wartosc(razy(zero, malo), 0));
  
  wartosc duzo = podzielic(jeden, malo);
  printf("inf=%lf\n", sr_wartosc(duzo));
  
  printf("1=%d\n", in_wartosc(razy(zero, duzo), 0));
  
  return 0;
  }
