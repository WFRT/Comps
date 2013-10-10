#include "Norm.h"
MetricNorm::MetricNorm(const Options& iOptions, const Data& iData) : MetricBasic(iOptions, iData) {
   iOptions.getRequiredValue("order", mOrder);
}
float MetricNorm::computeCore(float iObs, float iForecast) const {
   return pow(fabs(iObs - iForecast), mOrder);
}
