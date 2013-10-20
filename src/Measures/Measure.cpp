#include "SchemesHeader.inc"
#include "Measure.h"
#include "../Ensemble.h"

Measure::Measure(const Options& iOptions, const Data& iData) :
      Component(iOptions, iData),
      mAbsolute(false),
      mPrePower(1),
      mPostPower(1),
      mLastMeasure(Global::MV) {
   //! Forces measure to return the absolute value of the measure
   iOptions.getValue("absolute", mAbsolute);
   //! Raises the measure to this power
   iOptions.getValue("postPower", mPostPower);
   //! Raises the ensemble members to this power
   iOptions.getValue("prePower", mPrePower);
}

#include "Schemes.inc"

float Measure::measure(const Ensemble& iEnsemble) const {
   if(iEnsemble.getValues() == mLastEnsemble) {
      return mLastMeasure;
   }
   float value;
   if(mPrePower == 1) {
      value = measureCore(iEnsemble);
   }
   else {
      // Transform values of the ensemble
      Ensemble ens = iEnsemble;
      for(int i = 0; i < ens.size(); i++) {
         if(Global::isValid(ens[i])) {
            ens[i] = pow(ens[i], mPrePower);
         }
      }
      value = measureCore(ens);
   }

   if(mAbsolute) {
      value = std::fabs(value);
   }
   if(mPostPower != 1) {
      // Speed improvement if value^1 is not attempted?
      value = std::pow(value,mPostPower);
   }

   mLastMeasure = value;
   mLastEnsemble = iEnsemble.getValues();
   return value;
}
bool Measure::isPositive() const {
   return mAbsolute || isPositiveCore();
}
bool Measure::isPositiveDefinite() const {
   return isPositive() && isDefinite();
}
