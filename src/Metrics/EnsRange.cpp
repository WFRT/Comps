#include "EnsRange.h"
#include "../Distribution.h"
MetricEnsRange::MetricEnsRange(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {
}
float MetricEnsRange::computeCore(const Obs& iObs, const Forecast& iForecast) const {
   Ensemble ens = iForecast.getEnsemble();
   float xLower = ens.getMin();
   float xUpper = ens.getMax();
   if(!Global::isValid(xLower) || !Global::isValid(xUpper))
      return Global::MV;
   else
      return xUpper - xLower;
}
