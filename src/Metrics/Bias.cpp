#include "Bias.h"
MetricBias::MetricBias(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {

}
float MetricBias::compute(int iDate,
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
      return fcstValue - obsValue;
}

std::string MetricBias::getName() const {
   return "Bias";
}
