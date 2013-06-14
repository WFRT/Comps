#ifndef METRIC_MEASURE_H
#define METRIC_MEASURE_H
#include "Metric.h"
#include "../Measures/Measure.h"
class MetricMeasure : public Metric {
   public:
      MetricMeasure(const Options& iOptions, const Data& iData);
      ~MetricMeasure();
      float compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const;
      std::string getName() const;
   private:
      const Measure* mMeasure;
};
#endif
