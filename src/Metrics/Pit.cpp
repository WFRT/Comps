#include "Pit.h"
#include "../Variables/Variable.h"
#include "../Distribution.h"
MetricPit::MetricPit(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {

}
float MetricPit::computeCore(const Obs& iObs, const Forecast& iForecast) const {
   float       obsValue = iObs.getValue();
   const Variable* var = Variable::get(iObs.getVariable());

   Distribution::ptr dist = iForecast.getDistribution();
   float pit = dist->getCdf(obsValue);

   // Randomize pit if on the lower boundary

   if(!Global::isValid(pit)) {
      return Global::MV;
   }
   if(var->isLowerDiscrete() && var->getMin() == obsValue) {
      pit = Global::getRand() * pit;
   }
   else if(var->isUpperDiscrete() && var->getMax() == obsValue) {
      pit = 1 - Global::getRand() * (1-pit);
   }
   return pit;
}
