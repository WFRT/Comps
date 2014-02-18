#include "Crps.h"
#include "../Variables/Variable.h"
#include "../Distribution.h"
MetricCrps::MetricCrps(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {
}
float MetricCrps::computeCore(const Obs& iObs, const Distribution::ptr iForecast) const {
   // TODO
   float obs = iObs.getValue();
   const Variable* var = Variable::get(iObs.getVariable());

   // Setting minX and maxX: Use X where Cdf = 0.01 and 0.99?
   float total = 0;

   //float minX = iForecast->getInv(0.001);
   //float maxX = iForecast->getInv(0.999);
   float minX  = var->getMin();
   float maxX  = var->getMax();
   //std::cout << "CRPS: " << minX << " " << maxX << std::endl;
   //minX = minX < var->getMin() ? var->getMin() : minX;
   //maxX = maxX > var->getMax() ? var->getMax() : maxX;
   int   nX    = 1000;
   float dX    = (maxX - minX)/((float) nX);

   for(int i = 0; i < nX; i++) {
      float x = minX + i*dX;
      float cdf = iForecast->getCdf(x);
      if(!Global::isValid(cdf)) {
         total = Global::MV;
         break;
      }
      if(obs > x) {
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
