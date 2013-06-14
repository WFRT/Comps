#include "Forecast.h"
#include "../Distribution.h"
MetricForecast::MetricForecast(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {

}
float MetricForecast::compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const {
   Location    location = iObs.getLocation();
   float       offset   = iObs.getOffset();
   std::string variable = iObs.getVariable();
   float       obsValue = iObs.getValue();
   float fcstValue = iConfiguration.getDeterministic(iDate, iInit, iOffset, location, variable);
   if(!Global::isValid(fcstValue))
      return Global::MV;
   else
      return fcstValue;
}

std::string MetricForecast::getName() const {
   return "Forecast";
}
