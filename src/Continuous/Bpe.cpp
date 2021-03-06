#include "Continuous.h"
#include "Bpe.h"
#include "../Variables/Variable.h"
#include "../Interpolators/Step.h"
ContinuousBpe::ContinuousBpe(const Options& iOptions, const Data& iData) : Continuous(iOptions, iData),
      mInterpolator(NULL),
      mUseStep(false) {
   // Note about interpolation
   // The classical approach is to count fraction of members below the threshold. In this case the
   // Cdfs are:
   //    0    (below ens)
   //    0.25 (between 1st pair)
   //    0.50 (between 2nd pair)
   //    0.75 (between 3rd pair)
   //    1    (above ens)
   // To achieve this, a step interpolator is used with points (0.25, ens1), ... (1, ens4)
   //
   // Linear interpolation can also be used. In this case the interpolation points are different:
   // (0, ens1), (0.333, ens2), (0.667, ens3), (1, ens4)
   // The difference in the x points is handled by mUseStep

   std::string interpolatorTag;
   //! Tag of interpolation scheme to use between ensemble members
   //! If not specified, use a step function
   if(iOptions.getValue("interp", interpolatorTag))
      mInterpolator = Interpolator::getScheme(interpolatorTag);
   else {
      mUseStep = true;
      mInterpolator = new InterpolatorStep(Options());
   }

   mHandleOutside = false;
   iOptions.check();
}
ContinuousBpe::~ContinuousBpe() {
   delete mInterpolator;
}

float ContinuousBpe::getCdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   std::vector<float> x;
   std::vector<float> y;
   getXY(iEnsemble, x, y);

   const Variable* var = Variable::get(iEnsemble.getVariable());
   // On the boundaries, use the count of members, instead of interpolating
   if(   (var->isLowerDiscrete() && var->getMin() == iX)
      || (var->isUpperDiscrete() && var->getMax() == iX)) {
      int num = 0;
      int total = 0;
      for(int i = 0; i < iEnsemble.size(); i++) {
         float value = iEnsemble[i];
         if(Global::isValid(value)) {
            if(value == iX)
               num++;
            total++;
         }
      }
      if(total == 0)
         return Global::MV;
      if(total == 1 && num == 1 && var->getMin() == iX)
         return 1;
      if(total == 1 && num == 1 && var->getMax() == iX)
         return 0;

      if(num == 0 && var->getMin() == iX)
         return 0;
      else if(num == 0 && var->getMax() == iX)
         return 1;

      if(var->getMin() == iX) {
         // Lower boundary
         if(mUseStep)
            return (float) num/total;
         else
            return (float) (num-1)/(total-1);
      }
      else {
         // Upper boundary
         if(mUseStep)
            return (float) 1-num/total;
         else
            return (float) 1-(num-1)/(total-1);
      }
   }

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
      if(mUseStep)
         iY.push_back((float) i/(Nvalid));
      else
         iY.push_back((float) (i-1)/(Nvalid-1));
   }
   assert(iX.size() == iY.size());
}
