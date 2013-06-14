#ifndef METRIC_SELECTOR_SKILL_H
#define METRIC_SELECTOR_SKILL_H
#include "Metric.h"
class MetricSelectorSkill : public Metric {
   public:
      enum Type {typeMin = 0, typeMean = 10, typeMax = 20};
      MetricSelectorSkill(const Options& iOptions, const Data& iData);
      float compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const;
      std::string getName() const;
   private:
      Type mType;
};
#endif
