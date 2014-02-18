#ifndef METRIC_CRPS_H
#define METRIC_CRPS_H
#include "Metric.h"
class MetricCrps : public Metric {
   public:
      MetricCrps(const Options& iOptions, const Data& iData);
   private:
      float computeCore(const Obs& iObs, const Distribution::ptr iForecast) const;
};
#endif
