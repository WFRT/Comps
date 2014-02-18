#include "Pit.h"
#include "../Variables/Variable.h"
#include "../Distribution.h"
MetricPit::MetricPit(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {

}
float MetricPit::computeCore(const Obs& iObs, const Distribution::ptr iForecast) const {
   float       obsValue = iObs.getValue();
   const Variable* var = Variable::get(iObs.getVariable());

   float pit = iForecast->getCdf(obsValue);

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
   if(pit == 1 || pit == 0) {
      std::stringstream ss;
      ss << "MetricPit: Pit value of " << pit << " for D" << iObs.getDate() << " O" << iObs.getOffset() << " L" << iObs.getLocation().getId() << " V" << iObs.getVariable() << " obs = " << obsValue << std::endl;
      Global::logger->write(ss.str(), Logger::message);
   }
   return pit;
}
