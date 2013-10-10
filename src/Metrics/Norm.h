#ifndef METRIC_NORM_H
#define METRIC_NORM_H
#include "Metric.h"
class MetricNorm : public MetricBasic {
   public:
      MetricNorm(const Options& iOptions, const Data& iData);
   private:
      float computeCore(float iObs, float iForecast) const;
      int   mOrder;
};
#endif
