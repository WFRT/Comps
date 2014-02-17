#include "Metric.h"
#include "SchemesHeader.inc"
#include "../Data.h"

Metric::Metric(const Options& iOptions, const Data& iData) : Component(iOptions), mData(iData) {}

#include "Schemes.inc"

void Metric::compute(const Obs& iObs, const Forecast& iForecast, Score& iScore) const {
   float score;
   if((needsValidObs()  && !Global::isValid(iObs.getValue())) ||
      // TODO: iForecast should have an isValid function
      (needsValidFcst() && !Global::isValid(iForecast.getDeterministic().getValue())))
      score = Global::MV;
   else
      score = computeCore(iObs, iForecast);
   iScore = Score(getTag(), score, iForecast.getDate(), iForecast.getInit(), iForecast.getOffset(), iForecast.getLocation(), iForecast.getVariable());
}

MetricBasic::MetricBasic(const Options& iOptions, const Data& iData) : Metric(iOptions, iData), 
      mUseMedian(false), mAnomaly(false) {
   iOptions.getValue("useMedian", mUseMedian);
   //! Should the forecast and observation be relative to the climatological mean?
   iOptions.getValue("anomaly", mAnomaly);
}

float MetricBasic::computeCore(const Obs& iObs, const Forecast& iForecast) const {
   float obs = iObs.getValue();
   float fcst;
   if(mUseMedian) {
      fcst = iForecast.getDistribution()->getInv(0.5);
   }
   else {
      fcst = iForecast.getDeterministic().getValue();
   }

   if(mAnomaly) {
      // Subtract climatological value
      Location locations = iObs.getLocation();
      float clim = mData.getClim(iObs.getDate(), iObs.getInit(), iObs.getOffset(), iObs.getLocation(), iObs.getVariable());
      if(Global::isValid(clim)) {
         obs  -= clim;
         fcst -= clim;
      }
      else {
         return Global::MV;
      }
   }
   return computeCore(obs, fcst);
}
