#include "Averager.h"
#include "Measure.h"

AveragerMeasure::AveragerMeasure(const Options& iOptions, const Data& iData) : Averager(iOptions, iData) {
   std::string measureTag;
   iOptions.getRequiredValue("measure", measureTag);
   mMeasure = Measure::getScheme(measureTag, mData);
}
AveragerMeasure::~AveragerMeasure() {
   delete mMeasure;
}

float AveragerMeasure::average(const Ensemble& iValues, const Parameters& iParameters) const {
   return mMeasure->measure(iValues);
}
