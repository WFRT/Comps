#include "Metric.h"
#include "SchemesHeader.inc"

Metric::Metric(const Options& iOptions, const Data& iData) : Component(iOptions, iData) {}

#include "Schemes.inc"

void Metric::compute(const Obs& iObs, const Forecast& iForecast, Score& iScore) const {
   float score;
   if(needsValidObs()  && !Global::isValid(iObs.getValue()) ||
      // TODO: iForecast should have an isValid function
      needsValidFcst() && !Global::isValid(iForecast.getDeterministic().getValue()))
      score = Global::MV;
   else
      score = computeCore(iObs, iForecast);
   iScore = Score(getTag(), score, iForecast.getDate(), iForecast.getInit(), iForecast.getOffset(), iForecast.getLocation(), iForecast.getVariable());
}

MetricBasic::MetricBasic(const Options& iOptions, const Data& iData) : Metric(iOptions, iData), 
      mUseMedian(false) {
   iOptions.getValue("useMedian", mUseMedian);
}

float MetricBasic::computeCore(const Obs& iObs, const Forecast& iForecast) const {
   float fcst;
   if(mUseMedian) {
      fcst = iForecast.getDistribution()->getInv(0.5);
   }
   else {
      fcst = iForecast.getDeterministic().getValue();
   }
   return computeCore(iObs.getValue(), fcst);
}
