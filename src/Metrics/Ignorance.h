#ifndef METRIC_IGNORANCE_H
#define METRIC_IGNORANCE_H
#include "Metric.h"
class MetricIgnorance : public Metric {
   public:
      MetricIgnorance(const Options& iOptions, const Data& iData);
   private:
      float computeCore(const Obs& iObs, const Forecast& iForecast) const;
};
#endif
