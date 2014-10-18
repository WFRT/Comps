#include "Spread.h"
#include "../Distribution.h"
MetricSpread::MetricSpread(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {
   iOptions.check();
}

float MetricSpread::computeCore(const Obs& iObs, const Distribution::ptr iForecast) const {
   return iForecast->getMoment(2);
}
