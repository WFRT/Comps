#include "Quantile.h"
#include "../Distribution.h"
MetricQuantile::MetricQuantile(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {
   //! Compute threshold for this quantile (e.g. 0.1 for 10th percentile)
   iOptions.getRequiredValue("quantile", mQuantile);
   if(!Global::isValid(mQuantile) || mQuantile < 0 || mQuantile > 1) {
      std::stringstream ss;
      ss << "Quantile must be between 0 and 1";
      Global::logger->write(ss.str(), Logger::error);
   }
   iOptions.check();
}
float MetricQuantile::computeCore(const Obs& iObs, const Distribution::ptr iForecast) const {
   return iForecast->getInv(mQuantile);
}
