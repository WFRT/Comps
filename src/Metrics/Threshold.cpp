#include "Threshold.h"
#include "../Distribution.h"
MetricThreshold::MetricThreshold(const Options& iOptions, const Data& iData) : Metric(iOptions, iData),
      mUseMedian(false),
      mUseProbabilistic(false) {
   iOptions.getRequiredValue("threshold", mThreshold);
   iOptions.getValue("useMedian", mUseMedian);
   iOptions.getValue("useProbabilistic", mUseProbabilistic);
   if(mUseProbabilistic && mUseMedian) {
      Global::logger->write("MetricThreshold: Both 'useProbabilistic' and 'useMedian' cannot be specified", Logger::error);
   }
}
float MetricThreshold::compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const {
   Location    location = iObs.getLocation();
   float       offset   = iObs.getOffset();
   std::string variable = iObs.getVariable();
   float       obsValue = iObs.getValue();
   float fcstValue;

   Distribution::ptr dist = iConfiguration.getDistribution(iDate, iInit, iOffset, location, variable);
   if(mUseProbabilistic) {
      return dist->getCdf(mThreshold);
   }
   if(mUseMedian) {
      fcstValue = dist->getInv(0.5);
   }
   else {
      fcstValue = iConfiguration.getDeterministic(iDate, iInit, iOffset, location, variable);
   }
   if(!Global::isValid(fcstValue) || !Global::isValid(obsValue)) {
      return Global::MV;
   }
   if(fcstValue > mThreshold && obsValue > mThreshold ||
      fcstValue < mThreshold && obsValue < mThreshold || 
      fcstValue == mThreshold && obsValue == mThreshold)
      return 1;
   else
      return 0;
}

std::string MetricThreshold::getName() const {
   return "Threshold";
}
