#ifndef METRIC_OBS_H
#define METRIC_OBS_H
#include "Metric.h"
class MetricObs : public MetricBasic {
   public:
      MetricObs(const Options& iOptions, const Data& iData);
   private:
      float computeCore(float iObs, float iForecast) const;
      bool  needsValidFcst() const {return false;};
};
#endif
