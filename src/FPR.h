#pragma once

#include "definitions.h"
// #include "FP_UTILS.h"
#include "InputCheckers.h"
#include "random_number_generator.h"
using namespace std;

// floating-point range
class FPR {
public:
  HFP_TYPE lb;
  HFP_TYPE ub;
  unsigned int n_pars; // number of partitions
  unsigned int index_pars; // the index of partitions

  FPR () { // WARNING: this constructor gives you an invalid FPR!!
    lb = 1;
    ub = -1;
  }

  FPR (HFP_TYPE inlb, HFP_TYPE inub, unsigned int in_index_pars, unsigned int in_n_pars) {
    assert(inlb <= inub);
    assert(in_n_pars > 0);
    assert(in_index_pars >= 0 && in_index_pars < in_n_pars);
    lb = inlb;
    ub = inub;
    n_pars = in_n_pars;
    index_pars = in_index_pars; 
  }

  FPR & operator = (const FPR &rhs) {
    if (this == &rhs) return *this;
    this->lb = rhs.lb;
    this->ub = rhs.ub;
    this->n_pars = rhs.n_pars;
    this->index_pars = rhs.index_pars;
    return *this;
  }

  HFP_TYPE getLB () const { 
    return lb + (ub - lb) * ((HFP_TYPE)(index_pars) / (HFP_TYPE)(n_pars)); 
  }

  HFP_TYPE getUB () const { 
    return lb + (ub - lb) * ((HFP_TYPE)(index_pars + 1) / (HFP_TYPE)(n_pars)); 
  }
};

// typdef and function prototype
typedef std::vector<std::pair<unsigned int, FPR> > CONF;

void sampleCONF (const string &outname, 
		 const CONF &conf, 
		 const ENUM_RANDOM_FUNC rfunc = NAIVE_RANDOM_FUNC, 
		 const char *fopen_mode="w") {
  FILE *outfile = fopen(outname.c_str(), fopen_mode);

  for (unsigned int vi = 0 ; vi < conf.size() ; vi++) {
    assert(vi == conf[vi].first);

    HFP_TYPE this_lb = conf[vi].second.getLB(); 
    HFP_TYPE this_ub = conf[vi].second.getUB(); 
    INPUTV_TYPE this_input = (INPUTV_TYPE) random128(this_lb, this_ub, rfunc); 
    fwrite(&this_input, sizeof(INPUTV_TYPE), 1, outfile); 
  }

  fclose(outfile);
}
