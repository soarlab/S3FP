#include <stdio.h>
#include <assert.h>
extern "C" {
#include <quadmath.h>
}

#ifndef FT
#define FT float
#endif 

#ifndef IFT
#define IFT float 
#endif 

#ifndef OFT
#define OFT __float128 
#endif 



int main (int argc, char *argv[]) {
  assert(sizeof(FT) == sizeof(float) ||
	 sizeof(FT) == sizeof(double) || 
	 sizeof(FT) == sizeof(__float128));
  assert(argc == 3);
  char *inname = argv[1];
  char *outname = argv[2];

  unsigned index[2];
  unsigned int i;
  IFT idata;
  OFT odata; 

  FILE *infile = fopen(inname, "r"); 
  FILE *outfile = fopen(outname, "w");
  assert(infile != NULL && outfile != NULL); 
  fseek(infile, 0, SEEK_END); 
  unsigned long fsize = ftell(infile); 
  assert(fsize % sizeof(IFT) == 0);
  unsigned long N = fsize / sizeof(IFT); 
  fseek(infile, 0, SEEK_SET); 
  assert(N == 4); 

  fread(&idata, sizeof(IFT), 1, infile);
  FT a = (FT) idata;
  fread(&idata, sizeof(IFT), 1, infile);
  FT b = (FT) idata; 
  fread(&idata, sizeof(IFT), 1, infile);
  FT c = (FT) idata; 
  fread(&idata, sizeof(IFT), 1, infile);
  FT d = (FT) idata; 

  FT rel = (a - b) / (c + d); 

  odata = (OFT) rel; 
  fwrite(&odata, sizeof(OFT), 1, outfile);
  fclose(outfile);

  return 0;
}

  
