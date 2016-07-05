#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include <string.h>
#include <assert.h> 
#include "quadmath.h" 

void InvalidUsage () {
  printf("Invalid Usage...\n"); 
  printf("Usage: show-fp-stream [floating-point bit-width] [stream file name]\n"); 
  exit(1); 
}

int main (int argc, char *argv[]) {
  if (argc != 3) 
    InvalidUsage(); 

  unsigned int fp_bitwidth = atoi(argv[1]); 
  if (!(fp_bitwidth == 32 || 
	fp_bitwidth == 64 || 
	fp_bitwidth == 128)) 
    InvalidUsage(); 

  FILE *ifile = fopen(argv[2], "r"); 
  assert(ifile != NULL); 

  fseek(ifile, 0, SEEK_END); 
  unsigned long fsize = ftell(ifile); 
  assert(fsize % (fp_bitwidth / 8) == 0); 
  fsize = fsize / (fp_bitwidth / 8); 
  fseek(ifile, 0, SEEK_SET); 

  float if32; 
  double if64; 
  __float128 if128; 
  float fmant32; 
  double fmant64;
  int fexpo; 
  for (unsigned long i = 0 ; i < fsize ; i++) {
    switch (fp_bitwidth) {
    case 32: 
      fread(&if32, sizeof(float), 1, ifile); 
      fmant32 = frexpf(if32, &fexpo); 
      printf("[%lu]: %21.20f = %31.30f * 2^%d \n", 
	     i, (double)if32, fmant32, fexpo); 
      break; 
    case 64:
      fread(&if64, sizeof(double), 1, ifile); 
      fmant64 = frexp(if64, &fexpo); 
      printf("[%lu]: %21.20f = %31.30f * 2^%d \n", 
	     i, (double)if64, fmant64, fexpo);  
      break; 
    case 128: 
      fread(&if128, sizeof(__float128), 1, ifile); 
      printf("[%lu]: %21.20f\n", i, (double)if128); 
      break; 
    default:
      assert(false); 
      break; 
    }
  }

  fclose(ifile); 

  return 0; 
}
