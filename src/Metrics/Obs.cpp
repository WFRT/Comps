#include "Obs.h"
MetricObs::MetricObs(const Options& iOptions, const Data& iData) : MetricBasic(iOptions, iData) {}
float MetricObs::computeCore(float iObs, float iForecast) const {
   return iObs;
}
