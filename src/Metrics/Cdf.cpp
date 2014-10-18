#include "Cdf.h"
#include "../Distribution.h"
MetricCdf::MetricCdf(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {
   //! Compute CDF for this threshold (e.g. 0 mm for precipitation)
   iOptions.getRequiredValue("threshold", mThreshold);
   iOptions.check();
}
float MetricCdf::computeCore(const Obs& iObs, const Distribution::ptr iForecast) const {
   return iForecast->getCdf(mThreshold);
}
