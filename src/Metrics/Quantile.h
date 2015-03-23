#ifndef METRIC_QUANTILE_H
#define METRIC_QUANTILE_H
#include "Metric.h"
//! Computes the threshold corresponding to a specific quantile
class MetricQuantile : public Metric {
   public:
      MetricQuantile(const Options& iOptions, const Data& iData);
   private:
      float computeCore(const Obs& iObs, const Distribution::ptr iForecast) const;
      bool needsValidObs()  const {return false;};
      float mQuantile;
};
#endif
