#ifndef METRIC_MAE_H
#define METRIC_MAE_H
#include "Metric.h"
class MetricMae : public Metric {
   public:
      MetricMae(const Options& iOptions, const Data& iData);
      float compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const;
      std::string getName() const;
   private:
      bool mUseMedian;
};
#endif
