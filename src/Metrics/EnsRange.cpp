#include "EnsRange.h"
#include "../Distribution.h"
MetricEnsRange::MetricEnsRange(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {
}
float MetricEnsRange::compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const {
   Location    location = iObs.getLocation();
   std::string variable = iObs.getVariable();
   Ensemble ens;
   iConfiguration.getEnsemble(iDate, iInit, iOffset, location, variable, ens);
   float xLower = ens.getMin();
   float xUpper = ens.getMax();
   if(!Global::isValid(xLower) || !Global::isValid(xUpper))
      return Global::MV;
   else
      return xUpper - xLower;
}

std::string MetricEnsRange::getName() const {
   return "EnsRange";
}
