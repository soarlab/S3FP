#include <iostream> 
#include <stdio.h> 
#include <assert.h> 
#include <math.h> 
extern "C" {
#include "quadmath.h" 
} 


#ifndef IFT 
#define IFT float 
#endif 

#ifndef OFT 
#define OFT __float128
#endif 

#ifndef PI 
#define PI 3.1415926
#endif 


using namespace std; 


int main (int argc, char *argv[]) {
  assert(argc == 3); 
  
  char *iname = argv[1]; 
  char *oname = argv[2]; 

  FILE *ifile = fopen(iname, "r"); 
  FILE *ofile = fopen(oname, "w"); 

  assert(ifile != NULL && ofile != NULL); 

  fseek(ifile, 0, SEEK_END); 
  unsigned long fsize = ftell(ifile); 
  assert(fsize == (sizeof(IFT) * 2)); 
  fseek(ifile, 0, SEEK_SET); 

  IFT in_r, in_h; 

  fread(&in_r, sizeof(IFT), 1, ifile); 
  fread(&in_h, sizeof(IFT), 1, ifile); 

  FT r = in_r; 
  FT h = in_h; 
  FT rel = 0; 

  if      (sizeof(IFT) == sizeof(32)) {
    rel = PI * r * (r + sqrtf(h*h + r*r)); 
  } 
  else if (sizeof(IFT) == sizeof(64)) {
    rel = PI * r * (r + sqrt(h*h + r*r)); 
  }
  else assert(false && "Error: Invalid type of IFT..."); 
  
  OFT outv = rel; 
  fwrite(&outv, sizeof(OFT), 1, ofile); 
  
  fclose(ifile); 
  fclose(ofile); 

  return 0; 
}
