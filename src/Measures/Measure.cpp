#include "SchemesHeader.inc"
#include "Measure.h"
#include "../Ensemble.h"

Measure::Measure(const Options& iOptions, const Data& iData) :
      Component(iOptions, iData),
      mAbsolute(false),
      mPower(1),
      mLastMeasure(Global::MV) {
   iOptions.getValue("absolute", mAbsolute);
   iOptions.getValue("power", mPower);
}

#include "Schemes.inc"

float Measure::measure(const Ensemble& iEnsemble, const Parameters& iParameters) const {
   if(iEnsemble.getValues() == mLastEnsemble) {
      return mLastMeasure;
   }
   float value = measureCore(iEnsemble, iParameters);
   if(isPositive()) {
      value = std::fabs(value);
   }
   // Speed improvement if value^1 is not attempted?
   if(mPower != 1)
      value = std::pow(value,mPower);

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
