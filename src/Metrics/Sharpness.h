#ifndef METRIC_SHARPNESS_H
#define METRIC_SHARPNESS_H
#include "Metric.h"
class MetricSharpness : public Metric {
   public:
      MetricSharpness(const Options& iOptions, const Data& iData);
   private:
      float computeCore(const Obs& iObs, const Forecast& iForecast) const;
      bool  needsValidObs() const {return false;};
      float mWidth;
};
#endif
