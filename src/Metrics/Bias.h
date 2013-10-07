#ifndef METRIC_BIAS_H
#define METRIC_BIAS_H
#include "Metric.h"
//! Forecast (deterministic) minus observation
class MetricBias : public Metric {
   public:
      MetricBias(const Options& iOptions, const Data& iData);
      float compute(const Obs& iObs, const Forecast& iForecast, Score& iScore) const;
      std::string getName() const;
};
#endif
