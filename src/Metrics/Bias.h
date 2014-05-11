#ifndef METRIC_BIAS_H
#define METRIC_BIAS_H
#include "Metric.h"
//! Observation minus forecast (deterministic)
class MetricBias : public MetricBasic {
   public:
      MetricBias(const Options& iOptions, const Data& iData);
   private:
      float computeCore(float iObs, float iForecast) const;
};
#endif
