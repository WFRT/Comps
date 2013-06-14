#include "Averager.h"
#include "Regression.h"

AveragerRegression::AveragerRegression(const Options& iOptions, const Data& iData) : Averager(iOptions, iData) {
   Component::underDevelopment();
}

float AveragerRegression::average(const Ensemble& iValues, const Parameters& iParameters) const {
   assert(iParameters.size() == iValues.size());
   float value = 0;
   float total = 0;
   for(int i = 0; i < (int) iValues.size(); i++) {
      assert(iParameters[i] != Global::MV);
      if(iValues[i] != Global::MV) {
         value += iValues[i];
         total += iParameters[i];
      }
   }
   if(total != 0) {
      value = value / total;
   }
   else  {
      value = Global::MV;
   }
   return value;
}
