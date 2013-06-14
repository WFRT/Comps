#ifndef METRIC_THRESHOLD_H
#define METRIC_THRESHOLD_H
#include "Metric.h"
class MetricThreshold : public Metric {
   public:
      MetricThreshold(const Options& iOptions, const Data& iData);
      float compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const;
      std::string getName() const;
   private:
      float mThreshold;
      bool mUseMedian;
      bool mUseProbabilistic;
};
#endif
