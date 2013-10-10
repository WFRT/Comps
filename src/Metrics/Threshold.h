#ifndef METRIC_THRESHOLD_H
#define METRIC_THRESHOLD_H
#include "Metric.h"
class MetricThreshold : public MetricBasic {
   public:
      MetricThreshold(const Options& iOptions, const Data& iData);
   private:
      float computeCore(float iObs, float iForecast) const;
      float mThreshold;
};
#endif
