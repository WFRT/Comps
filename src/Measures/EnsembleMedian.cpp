#include "EnsembleMedian.h"
#include "../Options.h"
#include "../Ensemble.h"

MeasureEnsembleMedian::MeasureEnsembleMedian(const Options& iOptions, const Data& iData) : Measure(iOptions, iData) {
}

float MeasureEnsembleMedian::measureCore(const Ensemble& iEnsemble) const {
   float median;
   // Remove missing values
   std::vector<float> temp;
   for(int i = 0; i < iEnsemble.size(); i++) {
      if(iEnsemble[i] != Global::MV) {
         temp.push_back(iEnsemble[i]);
      }
   }
   if(temp.size() == 0) {
      median = Global::MV;
   }
   else {
      std::sort(temp.begin(), temp.end());
      unsigned int N = temp.size();
      // Even size
      if(N % 2 == 0) {
         median = (temp[N/2 - 1] + temp[N/2])/2;
      }
      // Odd size
      else {
         median = temp[floor(N/2)];
      }
   }
   return median;
}
