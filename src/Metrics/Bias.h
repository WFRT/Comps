#ifndef METRIC_BIAS_H
#define METRIC_BIAS_H
#include "Metric.h"
class MetricBias : public Metric {
   public:
      MetricBias(const Options& iOptions, const Data& iData);
      float compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const;
      std::string getName() const;
};
#endif
