#include "Ignorance.h"
#include "../Distribution.h"
MetricIgnorance::MetricIgnorance(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {

}
float MetricIgnorance::computeCore(const Obs& iObs, const Distribution::ptr iForecast) const {
   float pdf = iForecast->getPdf(iObs.getValue());

   if(!Global::isValid(pdf))
      return Global::MV;
   else {
      float value = -log(pdf)/log(2);
      return value;
   }
}
