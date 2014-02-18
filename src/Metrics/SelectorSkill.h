#ifndef METRIC_SELECTOR_SKILL_H
#define METRIC_SELECTOR_SKILL_H
#include "Metric.h"
class MetricSelectorSkill : public Metric {
   public:
      enum Type {typeMin = 0, typeMean = 10, typeMax = 20};
      MetricSelectorSkill(const Options& iOptions, const Data& iData);
   private:
      float computeCore(const Obs& iObs, const Distribution::ptr iForecast) const;
      Type mType;
};
#endif
