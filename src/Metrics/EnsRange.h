#ifndef METRIC_ENS_RANGE_H
#define METRIC_ENS_RANGE_H
#include "Metric.h"
class MetricEnsRange : public Metric {
   public:
      MetricEnsRange(const Options& iOptions, const Data& iData);
      float compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const;
      std::string getName() const;
};
#endif
