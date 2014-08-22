#include "EnsSpread.h"
MetricEnsSpread::MetricEnsSpread(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {
   iOptions.check();
}
float MetricEnsSpread::computeCore(const Obs& iObs, const Distribution::ptr iForecast) const {
   Ensemble ens = iForecast->getEnsemble();
   return sqrt(ens.getMoment(2));
}
