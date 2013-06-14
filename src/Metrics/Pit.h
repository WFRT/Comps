#ifndef METRIC_PIT_H
#define METRIC_PIT_H
#include "Metric.h"
class MetricPit : public Metric {
   public:
      MetricPit(const Options& iOptions, const Data& iData);
      float compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const;
      std::string getName() const;
};
#endif
