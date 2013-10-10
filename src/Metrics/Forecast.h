#ifndef METRIC_FORECAST_H
#define METRIC_FORECAST_H
#include "Metric.h"
class MetricForecast : public MetricBasic {
   public:
      MetricForecast(const Options& iOptions, const Data& iData);
   private:
      float computeCore(float iObs, float iForecast) const;
      bool  needsValidObs() const {return false;};
};
#endif
