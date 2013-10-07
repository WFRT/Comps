#include "Bias.h"
MetricBias::MetricBias(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {}
float MetricBias::compute(const Obs& iForecast, const Forecast& iForecast) const {
   float obsValue = iObs.getValue();
   float fcstValue = iForecast.getDeterministic().getValue();
   return fcstValue - obsValue;
}

std::string MetricBias::getName() const {
   return "Bias";
}
