#include "Forecast.h"
#include "../Distribution.h"
MetricForecast::MetricForecast(const Options& iOptions, const Data& iData) : MetricBasic(iOptions, iData) {
   iOptions.check();
}
float MetricForecast::computeCore(float iObs, float iForecast) const {
   return iForecast;
}
