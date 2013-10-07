#ifndef METRIC_MAE_H
#define METRIC_MAE_H
#include "Metric.h"
class MetricMae : public Metric {
   public:
      MetricMae(const Options& iOptions, const Data& iData);
      void compute(const Obs& iForecast,
                   const Forecast& iForecast,
                   Score& iScore) const;
      std::string getName() const;
   private:
      bool mUseMedian;
};
#endif
