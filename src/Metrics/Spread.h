#ifndef METRIC_SPREAD_H
#define METRIC_SPREAD_H
#include "Metric.h"
class MetricSpread : public Metric {
   public:
      MetricSpread(const Options& iOptions, const Data& iData);
      float compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const;
      std::string getName() const;
};
#endif
