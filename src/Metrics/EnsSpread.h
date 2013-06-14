#ifndef METRIC_ENS_SPREAD_H
#define METRIC_ENS_SPREAD_H
#include "Metric.h"
class MetricEnsSpread : public Metric {
   public:
      MetricEnsSpread(const Options& iOptions, const Data& iData);
      float compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const;
      std::string getName() const;
};
#endif
