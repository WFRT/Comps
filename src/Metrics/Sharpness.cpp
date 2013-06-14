#include "Sharpness.h"
#include "../Distribution.h"
MetricSharpness::MetricSharpness(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {
   iOptions.getRequiredValue("width", mWidth);
   assert(mWidth > 0);
   assert(mWidth < 1);
}
float MetricSharpness::compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const {
   Location    location = iObs.getLocation();
   std::string variable = iObs.getVariable();
   Distribution::ptr dist = iConfiguration.getDistribution(iDate, iInit, iOffset, location, variable);
   float pLower = 0.5 - mWidth/2;
   float pUpper = 0.5 + mWidth/2;
   float xLower = dist->getInv(pLower);
   float xUpper = dist->getInv(pUpper);
   if(!Global::isValid(xLower) || !Global::isValid(xUpper))
      return Global::MV;
   else
      return xUpper - xLower;
}

std::string MetricSharpness::getName() const {
   return "Sharpness";
}
