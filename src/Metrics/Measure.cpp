#include "Measure.h"
#include "../Distribution.h"
MetricMeasure::MetricMeasure(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {
   std::string measureTag;
   iOptions.getValue("measure", measureTag);
   mMeasure = Measure::getScheme(measureTag, iData);
   iOptions.check();
}
MetricMeasure::~MetricMeasure() {
   delete mMeasure;
}
float MetricMeasure::computeCore(const Obs& iObs, const Distribution::ptr iForecast) const {
   return mMeasure->measure(iForecast->getEnsemble());
}
