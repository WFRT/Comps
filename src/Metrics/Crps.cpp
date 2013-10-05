#include "Crps.h"
#include "../Variables/Variable.h"
#include "../Distribution.h"
MetricCrps::MetricCrps(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {

}
float MetricCrps::compute(int iDate,
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

   // TODO
   const Variable* var = Variable::get(variable);

   // Setting minX and maxX: Use X where Cdf = 0.01 and 0.99?
   float total = 0;

   Distribution::ptr dist = iConfiguration.getDistribution(iDate, iInit, iOffset, location, variable);
   //float minX = dist->getInv(0.001);
   //float maxX = dist->getInv(0.999);
   float minX  = var->getMin();
   float maxX  = var->getMax();
   //std::cout << "CRPS: " << minX << " " << maxX << std::endl;
   //minX = minX < var->getMin() ? var->getMin() : minX;
   //maxX = maxX > var->getMax() ? var->getMax() : maxX;
   int   nX    = 1000;
   float dX    = (maxX - minX)/((float) nX);

   for(int i = 0; i < nX; i++) {
      float x = minX + i*dX;
      float cdf = dist->getCdf(x);
      if(!Global::isValid(cdf)) {
         total = Global::MV;
         break;
      }
      if(obsValue > x) {
         total += cdf*cdf;
      }
      else {
         total += (1-cdf)*(1-cdf);
      }
   }
   if(Global::isValid(total)) {
      total *= dX;
   }
   assert(Global::isMissing(total) || total >= 0);
   return total;
}

std::string MetricCrps::getName() const {
   return "Crps";
}
