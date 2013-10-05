#include "Measure.h"
#include "../Distribution.h"
MetricMeasure::MetricMeasure(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {
   std::string measureTag;
   iOptions.getValue("measure", measureTag);
   mMeasure = Measure::getScheme(measureTag, iData);
   if(mMeasure->needsTraining()) {
      std::stringstream ss;
      ss << "Measure " << mMeasure->getSchemeName() << " requires training, and can therefore not be used in metric " << getName();

   }
}
MetricMeasure::~MetricMeasure() {
   delete mMeasure;
}
float MetricMeasure::compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const {
   Location    location = iObs.getLocation();
   std::string variable = iObs.getVariable();
   float       obsValue = iObs.getValue();

   if(!Global::isValid(obsValue)) {
      return Global::MV;
   }
   Ensemble ens;
   iConfiguration.getEnsemble(iDate, iInit, iOffset, location, variable, ens);

   return mMeasure->measure(ens, Parameters());
}

std::string MetricMeasure::getName() const {
   return "Measure";
}
