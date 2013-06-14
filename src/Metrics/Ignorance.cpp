#include "Ignorance.h"
#include "../Distribution.h"
MetricIgnorance::MetricIgnorance(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {

}
float MetricIgnorance::compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const {
   Location    location = iObs.getLocation();
   float       offset   = iObs.getOffset();
   std::string variable = iObs.getVariable();
   float       obsValue = iObs.getValue();
   if(!Global::isValid(obsValue))
      return Global::MV;

   Distribution::ptr dist = iConfiguration.getDistribution(iDate, iInit, iOffset, location, variable);
   float pdf = dist->getPdf(obsValue);

   if(!Global::isValid(pdf))
      return Global::MV;
   else {
      float value = -log(pdf)/log(2);
      return value;
   }
}

std::string MetricIgnorance::getName() const {
   return "Ignorance";
}
