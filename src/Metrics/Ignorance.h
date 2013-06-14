#ifndef METRIC_IGNORANCE_H
#define METRIC_IGNORANCE_H
#include "Metric.h"
class MetricIgnorance : public Metric {
   public:
      MetricIgnorance(const Options& iOptions, const Data& iData);
      float compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const;
      std::string getName() const;
};
#endif
