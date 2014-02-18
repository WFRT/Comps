#ifndef METRIC_MEASURE_H
#define METRIC_MEASURE_H
#include "Metric.h"
#include "../Measures/Measure.h"
class MetricMeasure : public Metric {
   public:
      MetricMeasure(const Options& iOptions, const Data& iData);
      ~MetricMeasure();
   private:
      float computeCore(const Obs& iObs, const Distribution::ptr iForecast) const;
      const Measure* mMeasure;
};
#endif
