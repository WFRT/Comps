#include "Spread.h"
#include "../Distribution.h"
MetricSpread::MetricSpread(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {}

float MetricSpread::computeCore(const Obs& iObs, const Forecast& iForecast) const {
   return iForecast.getDistribution()->getMoment(2);
}
