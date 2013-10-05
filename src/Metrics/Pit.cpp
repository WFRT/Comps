#include "Pit.h"
#include "../Variables/Variable.h"
#include "../Distribution.h"
MetricPit::MetricPit(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {

}
float MetricPit::compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const {
   float       obsValue = iObs.getValue();
   if(!Global::isValid(obsValue))
      return Global::MV;

   Location    location = iObs.getLocation();
   std::string variable = iObs.getVariable();
   const Variable* var = Variable::get(variable);

   Distribution::ptr dist = iConfiguration.getDistribution(iDate, iInit, iOffset, location, variable);
   float pit = dist->getCdf(obsValue);

   // Randomize pit if on the lower boundary

   if(!Global::isValid(pit)) {
      return Global::MV;
   }
   if(var->isLowerDiscrete() && var->getMin() == obsValue) {
      pit = Global::getRand() * pit;
   }
   return pit;
}

std::string MetricPit::getName() const {
   return "Pit";
}
