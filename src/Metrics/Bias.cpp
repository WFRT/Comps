#include "Bias.h"
MetricBias::MetricBias(const Options& iOptions, const Data& iData) : MetricBasic(iOptions, iData) {}
float MetricBias::computeCore(float iObs, float iForecast) const {
   return iObs - iForecast;
}
