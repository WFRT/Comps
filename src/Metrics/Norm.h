#ifndef METRIC_NORM_H
#define METRIC_NORM_H
#include "Metric.h"
class MetricNorm : public Metric {
   public:
      MetricNorm(const Options& iOptions, const Data& iData);
      float compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const;
      std::string getName() const;
   private:
      int mOrder;
};
#endif
