#include "Metric.h"
#include "SchemesHeader.inc"

Metric::Metric(const Options& iOptions, const Data& iData) : Component(iOptions, iData) {}

#include "Schemes.inc"

float Metric::compute(const Obs& iObs, const Forecast& iForecast, Score& iScore) const {
   float score;
   if(needsValidObs()  && !Global::isValid(iObs.getValue()) ||
      needsValidFcst() && !Global::isValid(iForecast).isValid())
      score = Global::MV;
   else
      score = computeCore(iObs, iForecast);
   Score(score, iObs.getDate(), init, iObs.getOffset(), iObs.getLocation(), iObs.getVariable());
}
