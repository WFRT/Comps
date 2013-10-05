#include "Spread.h"
#include "../Distribution.h"
MetricSpread::MetricSpread(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {
}
float MetricSpread::compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const {
   Location    location = iObs.getLocation();
   std::string variable = iObs.getVariable();
   float       obsValue = iObs.getValue();

   if(!Global::isValid(obsValue)) {
      return Global::MV;
   }
   Distribution::ptr dist = iConfiguration.getDistribution(iDate, iInit, iOffset, location, variable);

   return dist->getMoment(2);
}

std::string MetricSpread::getName() const {
   return "Spread";
}
