#ifndef METRIC_SPREAD_H
#define METRIC_SPREAD_H
#include "Metric.h"
class MetricSpread : public Metric {
   public:
      MetricSpread(const Options& iOptions, const Data& iData);
   private:
      float computeCore(const Obs& iObs, const Forecast& iForecast) const;
};
#endif
