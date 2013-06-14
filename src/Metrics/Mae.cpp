#include "Mae.h"
#include "../Distribution.h"
MetricMae::MetricMae(const Options& iOptions, const Data& iData) : Metric(iOptions, iData),
      mUseMedian(false) {
   iOptions.getValue("useMedian", mUseMedian);
}
float MetricMae::compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const {
   Location    location = iObs.getLocation();
   float       offset   = iObs.getOffset();
   std::string variable = iObs.getVariable();
   float       obsValue = iObs.getValue();
   float fcstValue;
   if(mUseMedian) {
      Distribution::ptr dist = iConfiguration.getDistribution(iDate, iInit, iOffset, location, variable);
      fcstValue = dist->getInv(0.5);
   }
   else {
      fcstValue = iConfiguration.getDeterministic(iDate, iInit, iOffset, location, variable);
   }

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
