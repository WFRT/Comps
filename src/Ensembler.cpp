#include "Ensembler.h"
#include "Ensemble.h"

void Ensembler::getEnsemble(Distribution::ptr iDistribution, int iNumMembers, Ensemble& iEnsemble) {

   float startP = (float) 1/(iNumMembers+1);
   float dP = (float) 1/(iNumMembers+1);
   std::vector<float> values;
   for(int i = 0; i < iNumMembers; i++) {
      float cdf = startP + i*dP;
      float x = iDistribution->getInv(cdf);
      values.push_back(x);
   }
   iEnsemble.setValues(values);
}
