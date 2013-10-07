#include "Mae.h"
#include "../Distribution.h"
MetricMae::MetricMae(const Options& iOptions, const Data& iData) : Metric(iOptions, iData),
      mUseMedian(false) {
   iOptions.getValue("useMedian", mUseMedian);
}
float MetricMae::computeCore(const Obs& iObs, const Forecast& iForecast, const Configuration& iConfiguration) const {
   float obsValue = iObs.getValue();
   float fcstValue;
   if(mUseMedian)
      fcstValue = iForecast.getDistribution()->getInv(0.5);
   else
      fcstValue = iForecast.getDeterministic();

   if(!Global::isValid(fcstValue) || !Global::isValid(obsValue)) {
      return Global::MV;
   }
   else {
      return fabs(fcstValue - obsValue);
   }
}

std::string MetricMae::getName() const {
   return "Mae";
}
