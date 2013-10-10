#ifndef METRIC_CONTINGENCY_H
#define METRIC_CONTINGENCY_H
#include "Metric.h"
//! Returns number from 2x2 contingency table formed by a set threshold.
class MetricContingency : public MetricBasic {
   public:
      MetricContingency(const Options& iOptions, const Data& iData);
   private:
      float computeCore(float iObs, float iForecast) const;
      float mThreshold;
      std::string mQuadrant;
};
#endif
