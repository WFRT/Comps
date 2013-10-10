#ifndef METRIC_ENS_RANGE_H
#define METRIC_ENS_RANGE_H
#include "Metric.h"
class MetricEnsRange : public Metric {
   public:
      MetricEnsRange(const Options& iOptions, const Data& iData);
   private:
      float computeCore(const Obs& iObs, const Forecast& iForecast) const;
      bool  needsValidObs() const {return false;};
};
#endif
