#ifndef METRIC_PIT_H
#define METRIC_PIT_H
#include "Metric.h"
//! Probability integral transform value corresponding to the observation. In other words, the CDF
//! of the forecast at the verifying value.
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
