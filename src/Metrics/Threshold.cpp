#include "Threshold.h"
#include "../Distribution.h"
MetricThreshold::MetricThreshold(const Options& iOptions, const Data& iData) : MetricBasic(iOptions, iData) {
   iOptions.getRequiredValue("threshold", mThreshold);
}
float MetricThreshold::computeCore(float iObs, float iForecast) const {
   if(iForecast > mThreshold && iObs > mThreshold ||
      iForecast < mThreshold && iObs < mThreshold || 
      iForecast == mThreshold && iObs == mThreshold) {
      return 1;
   }
   else {
      return 0;
   }
}
