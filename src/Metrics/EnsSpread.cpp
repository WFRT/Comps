#include "EnsSpread.h"
MetricEnsSpread::MetricEnsSpread(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {
}
float MetricEnsSpread::computeCore(const Obs& iObs, const Forecast& iForecast) const {
   Ensemble ens = iForecast.getEnsemble();
   return sqrt(ens.getMoment(2));
}
