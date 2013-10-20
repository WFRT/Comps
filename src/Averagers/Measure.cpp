#include "Averager.h"
#include "Measure.h"

AveragerMeasure::AveragerMeasure(const Options& iOptions, const Data& iData) : Averager(iOptions, iData) {
   std::string measureTag;
   //! Use this ensemble measure to reduce ensemble to deterministic forecast.
   iOptions.getRequiredValue("measure", measureTag);
   mMeasure = Measure::getScheme(measureTag, mData);
}
AveragerMeasure::~AveragerMeasure() {
   delete mMeasure;
}

float AveragerMeasure::average(const Ensemble& iValues, const Parameters& iParameters) const {
   return mMeasure->measure(iValues);
}
