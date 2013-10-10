#include "Brier.h"
#include "../Variables/Variable.h"
#include "../Distribution.h"
#include "../Obs.h"
#include "../Data.h"
#include <iomanip>
MetricBrier::MetricBrier(const Options& iOptions, const Data& iData) :
      Metric(iOptions, iData),
      mAnomaly(false),
      mAnomalyAbove(false),
      mAnomalyBelow(false) {
   // Should anomaly threshold be used? I.e. Does the forecast/obs exceed a certain amount
   // (threshold) above or below the climatological mean?
   iOptions.getValue("anomaly", mAnomaly);
   iOptions.getValue("anomalyAbove", mAnomalyAbove);
   iOptions.getValue("anomalyBelow", mAnomalyBelow);
   iOptions.getRequiredValue("threshold", mThreshold);
   if(mAnomalyAbove || mAnomalyBelow) {
      if(mAnomaly)
         Global::logger->write("MetricBrier: Both 'anomaly' and one of 'anomalyAbove' or 'anomalyBelow' where specified, which is redundant.", Logger::warning);
      mAnomaly = true;
   }
   if(mAnomaly && !mAnomalyBelow && !mAnomalyAbove) {
      // If only 'anomaly' was specified
      mAnomalyAbove = true;
      mAnomalyBelow = true;
   }
}
float MetricBrier::computeCore(const Obs& iObs, const Forecast& iForecast) const {
   float obs = iObs.getValue();

   Distribution::ptr dist = iForecast.getDistribution();

   // Part1: Compute probability that obs is beyond threshold
   float P = Global::MV;
   bool isBeyond; // Is the obs above threshold, or more anomalous than mThreshold?
   if(mAnomaly) {
      // Find climatological value at day of year corresponding to obs
      float clim = mData.getClim(iObs.getDate(), 0, iObs.getOffset(), iObs.getLocation(), iObs.getVariable());
      if(!Global::isValid(clim)) {
         return Global::MV;
      }

      // Probability that obs is further away from clim than mThreshold
      float lower = clim-mThreshold;
      float upper = clim+mThreshold;

      // Check if we are within the variables range
      const Variable* var = Variable::get(iObs.getVariable());
      if(Global::isValid(var->getMin()) && lower < var->getMin())
         lower = var->getMin();
      if(Global::isValid(var->getMax()) && upper > var->getMax())
         lower = var->getMax();

      float P0 = 0;
      float P1 = 0;
      isBeyond = false;
      if(mAnomalyBelow) {
         P0 = dist->getCdf(lower);     // Prob that obs is anomalously low
         isBeyond = isBeyond || (obs < lower);
      }
      if(mAnomalyAbove) {
         P1 = 1 - dist->getCdf(upper); // Prob that obs is anomalously high
         isBeyond = isBeyond || (obs > upper);
      }
      P = P1 + P0;
   }
   else {
      P = dist->getCdf(mThreshold);
      if(!Global::isValid(P)) {
         return Global::MV;
      }
      P = 1 - P;
      isBeyond = (obs > mThreshold);
   }

   // Part 2: Compute score
   float brier = Global::MV;
   if(Global::isValid(obs) && Global::isValid(P)) {
      if(isBeyond)
         brier = (1-P)*(1-P);
      else
         brier = P*P;
   }
   return brier;
}
