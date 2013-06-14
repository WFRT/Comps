#include "Continuous.h"
#include "Bpe.h"
#include "../BaseDistributions/BaseDistribution.h"
ContinuousBpe::ContinuousBpe(const Options& iOptions, const Data& iData) : Continuous(iOptions, iData) {
   std::string distributionTag;
   iOptions.getRequiredValue("distribution", distributionTag);
   Options optDistribution;
   Scheme::getOptions(distributionTag, optDistribution);
   mBaseDistribution = BaseDistribution::getScheme(optDistribution, iData);

   std::string interpolatorTag;
   iOptions.getRequiredValue("interp", interpolatorTag);
   Options optInterpolator;
   Scheme::getOptions(interpolatorTag, optInterpolator);
   mInterpolator = Interpolator::getScheme(optInterpolator, iData);
}
ContinuousBpe::~ContinuousBpe() {
   delete mBaseDistribution;
   delete mInterpolator;
}

float ContinuousBpe::getCdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   int N = iEnsemble.size();
   std::vector<float> x;
   std::vector<float> y;
   int Nvalid = 0;

   for(int i = 0; i < N; i++) {
      if(Global::isValid(iEnsemble[i])) {
         Nvalid++;
         x.push_back(iEnsemble[i]);
      }
   }
   
   for(int i = 1; i <= Nvalid; i++) {
      y.push_back((float) i/(Nvalid+1));
   }
   std::sort(x.begin(), x.end());
   assert(x.size() == y.size());
   float cdf = mInterpolator->interpolate(iX, x, y);

   return cdf;
}
float ContinuousBpe::getPdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   const std::vector<float>& values = iEnsemble.getValues();
   int N = Global::getNumValid(values);
   int nearestBelow = Global::getLowerIndex(iX, values);
   int nearestAbove = Global::getUpperIndex(iX, values);
   if(Global::isValid(nearestBelow) && Global::isValid(nearestAbove)) {
      return (float) 1.0/(values[nearestAbove] - values[nearestBelow])/N;
   }
   else if(!Global::isValid(nearestBelow) && !Global::isValid(nearestAbove)) {
      // Empty ensemble
      return Global::MV;
   }
   else {
      // No members below or above
      return 0;
   }
}

float ContinuousBpe::getInvCore(float iCdf, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   int N = iEnsemble.size();
   std::vector<float> x = iEnsemble.getValues();
   std::vector<float> y;
   
   for(int i = 1; i <= N; i++) {
      y.push_back((float) i/(N+1));
   }
   std::sort(x.begin(), x.end());
   assert(x.size() == y.size());
   float x0 = mInterpolator->interpolate(iCdf, y, x);

   return x0;
}
