#include "Sharpness.h"
#include "../Distribution.h"
MetricSharpness::MetricSharpness(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {
   iOptions.getRequiredValue("width", mWidth);
   if(mWidth <= 0 || mWidth >= 1) {
      std::stringstream ss;
      ss << "MetricSharpness: 'width' must be above 0 and below 1";
      Global::logger->write(ss.str(), Logger::error);
   }
}
float MetricSharpness::computeCore(const Obs& iObs, const Forecast& iForecast) const {
   Distribution::ptr dist = iForecast.getDistribution();
   float pLower = 0.5 - mWidth/2;
   float pUpper = 0.5 + mWidth/2;
   float xLower = dist->getInv(pLower);
   float xUpper = dist->getInv(pUpper);
   if(!Global::isValid(xLower) || !Global::isValid(xUpper))
      return Global::MV;
   else
      return xUpper - xLower;
}
