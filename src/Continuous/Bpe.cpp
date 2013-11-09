#include "Continuous.h"
#include "Bpe.h"
#include "../BaseDistributions/BaseDistribution.h"
ContinuousBpe::ContinuousBpe(const Options& iOptions, const Data& iData) : Continuous(iOptions, iData) {
   // TODO: Use a distribution outside the ensemble
   std::string distributionTag;
   // Tag of distribution to use outside ensemble
   // iOptions. getRequiredValue("distribution", distributionTag);
   // mBaseDistribution = BaseDistribution::getScheme(distributionTag, iData);

   std::string interpolatorTag;
   //! Tag of interpolation scheme to use between ensemble members
   iOptions.getRequiredValue("interp", interpolatorTag);
   mInterpolator = Interpolator::getScheme(interpolatorTag, iData);
}
ContinuousBpe::~ContinuousBpe() {
   //delete mBaseDistribution;
   delete mInterpolator;
}

float ContinuousBpe::getCdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   std::vector<float> x;
   std::vector<float> y;
   getXY(iEnsemble, x, y);

   if(x.size() == 0)
      return Global::MV;

   float cdf = Global::MV;
   // Below ensemble
   if(iX < x[0]) {
      // TODO:
      return 0;
   }
   // Above ensemble
   else if(iX > x[x.size()-1]) {
      // TODO:
      return 1;
   }
   // Inside ensemble
   else {
      if(x.size() == 1)
         return 0.5;
      cdf = mInterpolator->interpolate(iX, x, y);
   }

   return cdf;
}
float ContinuousBpe::getPdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   if(!Global::isValid(iEnsemble.getMin()) || !Global::isValid(iEnsemble.getMax()))
      return Global::MV;
   if(iX < iEnsemble.getMin())
      return 0;
   if(iX > iEnsemble.getMax())
      return 0;
   else {
      std::vector<float> x;
      std::vector<float> y;
      getXY(iEnsemble, x, y);
      return mInterpolator->slope(iX, x, y);
   }
}

float ContinuousBpe::getInvCore(float iCdf, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   std::vector<float> x;
   std::vector<float> y;
   getXY(iEnsemble, x, y);

   if(x.size() == 0)
      return Global::MV;

   float x0 = Global::MV;
   // Below ensemble
   if(iCdf <= y[0]) {
      // TODO:
      x0 = x[0];
   }
   // Above ensemble
   else if(iCdf >= y[y.size()-1]) {
      // TODO:
      x0 = x[x.size()-1];
   }
   // Inside ensemble
   else {
      x0 = mInterpolator->interpolate(iCdf, y, x);
   }

   return x0;
}

void ContinuousBpe::getXY(const Ensemble& iEnsemble, std::vector<float>& iX, std::vector<float>& iY) const {
   int N = iEnsemble.size();
   int Nvalid = 0;
   iX.clear();
   iY.clear();
   // Create x and y vectors for interpolation
   for(int i = 0; i < N; i++) {
      if(Global::isValid(iEnsemble[i])) {
         Nvalid++;
         iX.push_back(iEnsemble[i]);
      }
   }
   std::sort(iX.begin(), iX.end());
   
   for(int i = 1; i <= Nvalid; i++) {
      iY.push_back((float) i/(Nvalid+1));
   }
   assert(iX.size() == iY.size());
}
