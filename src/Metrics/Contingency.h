#ifndef METRIC_CONTINGENCY_H
#define METRIC_CONTINGENCY_H
#include "Metric.h"
//! Returns number from 2x2 contingency table formed by a set threshold.
class MetricContingency : public Metric {
   public:
      MetricContingency(const Options& iOptions, const Data& iData);
      float compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const;
      std::string getName() const;
   private:
      float mThreshold;
      std::string mQuadrant;
};
#endif
