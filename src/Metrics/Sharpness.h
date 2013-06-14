#ifndef METRIC_SHARPNESS_H
#define METRIC_SHARPNESS_H
#include "Metric.h"
class MetricSharpness : public Metric {
   public:
      MetricSharpness(const Options& iOptions, const Data& iData);
      float compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const;
      std::string getName() const;
   private:
      float mWidth;
};
#endif
