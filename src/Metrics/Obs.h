#ifndef METRIC_OBS_H
#define METRIC_OBS_H
#include "Metric.h"
class MetricObs : public Metric {
   public:
      MetricObs(const Options& iOptions, const Data& iData);
      float compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const;
      std::string getName() const;
};
#endif
