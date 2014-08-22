#include "Continuous.h"
#include "Upstream.h"
ContinuousUpstream::ContinuousUpstream(const Options& iOptions, const Data& iData) : Continuous(iOptions, iData) {
   Component::underDevelopment();
   /*
   std::string distributionTag;
   iOptions.getRequiredValue("distribution", distributionTag);
   Options optDistribution;
   Scheme::getOptions(distributionTag, optDistribution);
   mBaseDistribution = BaseDistribution::getScheme(optDistribution, iData);
  */
   iOptions.check();
}
/*
ContinuousUpstream::~ContinuousUpstream() {
   delete mBaseDistribution;
}
*/

float ContinuousUpstream::getCdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   // Find wind direction
   Ensemble windEnsemble = mData.getEnsemble(iEnsemble.getDate(), iEnsemble.getInit(), iEnsemble.getOffset(),
         iEnsemble.getLocation(), "WindDir");
   int count = 0;
   int total = 0;
   for(int i = 0; i < iEnsemble.size(); i++) {
      float currValue = iEnsemble[i];
      if(Global::isValid(currValue)) {
         if(currValue < iX) {
            count++;
         }
         total++;
      }
   }
   if(total == 0) {
      return Global::MV;
   }
   else {
      return (float) count/total;
   }
}
