#ifndef METRIC_FORECAST_H
#define METRIC_FORECAST_H
#include "Metric.h"
class MetricForecast : public Metric {
   public:
      MetricForecast(const Options& iOptions, const Data& iData);
      float compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const;
      std::string getName() const;
};
#endif
