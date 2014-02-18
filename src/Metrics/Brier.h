#ifndef METRIC_BRIER_H
#define METRIC_BRIER_H
#include "Metric.h"
class MetricBrier : public Metric {
   public:
      MetricBrier(const Options& iOptions, const Data& iData);
   private:
      float computeCore(const Obs& iObs, const Distribution::ptr iForecast) const;
      float mThreshold;
      bool  mAnomaly;
      bool  mAnomalyAbove;
      bool  mAnomalyBelow;
};
#endif
