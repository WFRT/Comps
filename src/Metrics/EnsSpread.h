#ifndef METRIC_ENS_SPREAD_H
#define METRIC_ENS_SPREAD_H
#include "Metric.h"
class MetricEnsSpread : public Metric {
   public:
      MetricEnsSpread(const Options& iOptions, const Data& iData);
   private:
      float computeCore(const Obs& iObs, const Distribution::ptr iForecast) const;
      bool  needsValidObs() const {return false;};
};
#endif
