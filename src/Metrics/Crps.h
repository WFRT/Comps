#ifndef METRIC_CRPS_H
#define METRIC_CRPS_H
#include "Metric.h"
class MetricCrps : public Metric {
   public:
      MetricCrps(const Options& iOptions, const Data& iData);
      float compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const;
      std::string getName() const;
};
#endif
