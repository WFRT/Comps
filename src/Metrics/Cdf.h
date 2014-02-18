#ifndef METRIC_CDF_H
#define METRIC_CDF_H
#include "Metric.h"
//! Computes the CDF at a specified threshold
class MetricCdf : public Metric {
   public:
      MetricCdf(const Options& iOptions, const Data& iData);
   private:
      float computeCore(const Obs& iObs, const Distribution::ptr iForecast) const;
      float mThreshold;
};
#endif
