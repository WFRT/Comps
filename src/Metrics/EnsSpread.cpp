#include "EnsSpread.h"
MetricEnsSpread::MetricEnsSpread(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {
}
float MetricEnsSpread::compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const {
   Location    location = iObs.getLocation();
   std::string variable = iObs.getVariable();
   Ensemble ens;
   iConfiguration.getEnsemble(iDate, iInit, iOffset, location, variable, ens);
   float variance = Global::variance(ens.getValues());
   if(!Global::isValid(variance))
      return Global::MV;
   else
      return sqrt(variance);
}

std::string MetricEnsSpread::getName() const {
   return "EnsSpread";
}
