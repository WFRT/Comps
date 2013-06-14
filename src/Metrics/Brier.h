#ifndef METRIC_BRIER_H
#define METRIC_BRIER_H
#include "Metric.h"
class MetricBrier : public Metric {
   public:
      MetricBrier(const Options& iOptions, const Data& iData);
      float compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const;
      std::string getName() const;
   private:
      float mThreshold;
      bool mAnomaly;
      bool mAnomalyAbove;
      bool mAnomalyBelow;
};
#endif
