#include "Measure.h"
#include "../Distribution.h"
MetricMeasure::MetricMeasure(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {
   std::string measureTag;
   iOptions.getValue("measure", measureTag);
   mMeasure = Measure::getScheme(measureTag, iData);
   if(mMeasure->needsTraining()) {
      std::stringstream ss;
      ss << "Measure " << mMeasure->getSchemeName() << " requires training, and can therefore not be used in metric " << getSchemeName();

   }
}
MetricMeasure::~MetricMeasure() {
   delete mMeasure;
}
float MetricMeasure::computeCore(const Obs& iObs, const Forecast& iForecast) const {
   return mMeasure->measure(iForecast.getEnsemble());
}
