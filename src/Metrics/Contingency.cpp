#include "Contingency.h"
#include "../Variables/Variable.h"
#include "../Distribution.h"
MetricContingency::MetricContingency(const Options& iOptions, const Data& iData) : MetricBasic(iOptions, iData) {
   //! Threshold to create contingency table for
   iOptions.getRequiredValue("threshold", mThreshold);
   if(!Global::isValid(mThreshold)) {
      std::stringstream ss;
      ss << "MetricContingency: 'threshold' is invalid.";
      Global::logger->write(ss.str(), Logger::error);
   }
   //! Which quadrant in contingency table? One of: hit, falseAlarm, correctRejection, and miss.
   iOptions.getRequiredValue("quadrant", mQuadrant);
   if(mQuadrant != "hit" && mQuadrant != "falseAlarm" &&
         mQuadrant != "correctRejection" && mQuadrant != "miss") {
      std::stringstream ss;
      ss << "MetricContingency: 'quadrant' must be one of: ";
      ss << "hit, falseAlarm, correctRejection, and miss";
      Global::logger->write(ss.str(), Logger::error);
   }
   iOptions.check();
}
float MetricContingency::computeCore(float iObs, float iForecast) const {
   if(mQuadrant == "hit") {
      return(iForecast >= mThreshold && iObs >= mThreshold);
   }
   else if(mQuadrant == "falseAlarm") {
      return(iForecast >= mThreshold && iObs < mThreshold);
   }
   else if(mQuadrant == "correctRejection") {
      return(iForecast < mThreshold && iObs < mThreshold);
   }
   else if(mQuadrant == "miss") {
      return(iForecast < mThreshold && iObs >= mThreshold);
   }
   // This should never happen
   return Global::MV;
}
