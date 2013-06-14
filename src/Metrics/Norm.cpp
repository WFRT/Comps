#include "Norm.h"
MetricNorm::MetricNorm(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {
   iOptions.getRequiredValue("order", mOrder);
}
float MetricNorm::compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const {
   Location    location = iObs.getLocation();
   float       offset   = iObs.getOffset();
   std::string variable = iObs.getVariable();
   float       obsValue = iObs.getValue();
   float fcstValue = iConfiguration.getDeterministic(iDate, iInit, iOffset, location, variable);
   if(!Global::isValid(fcstValue) || !Global::isValid(obsValue))
      return Global::MV;
   else
      return pow(fabs(fcstValue - obsValue), mOrder);
}

std::string MetricNorm::getName() const {
   return "Norm";
}
