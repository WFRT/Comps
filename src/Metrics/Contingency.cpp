#include "Contingency.h"
#include "../Variables/Variable.h"
#include "../Distribution.h"
MetricContingency::MetricContingency(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {
   //! Threshold to create contingency table for
   iOptions.getRequiredValue("threshold", mThreshold);
   //! Which quadrant in contingency table? One of: hit, falseAlarm, correctRejection, and miss.
   iOptions.getRequiredValue("quadrant", mQuadrant);
   if(mQuadrant != "hit" && mQuadrant != "falseAlarm" &&
         mQuadrant != "correctRejection" && mQuadrant != "miss") {
      std::stringstream ss;
      ss << "MetricContingency: 'quadrant' must be one of: ";
      ss << "hit, falseAlarm, correctRejection, and miss";
      Global::logger->write(ss.str(), Logger::error);
   }
}
float MetricContingency::compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const {
   Location    location = iObs.getLocation();
   std::string variable = iObs.getVariable();
   float       obsValue = iObs.getValue();
   float       fcstValue = iConfiguration.getDeterministic(iDate, iInit, iOffset, location, variable);
   if(!Global::isValid(obsValue) || !Global::isValid(fcstValue)) {
      return Global::MV;
   }

   if(mQuadrant == "hit") {
      return(fcstValue >= mThreshold && obsValue >= mThreshold);
   }
   else if(mQuadrant == "falseAlarm") {
      return(fcstValue >= mThreshold && obsValue < mThreshold);
   }
   else if(mQuadrant == "correctRejection") {
      return(fcstValue < mThreshold && obsValue < mThreshold);
   }
   else if(mQuadrant == "miss") {
      return(fcstValue < mThreshold && obsValue >= mThreshold);
   }
   // This should never happen
   return Global::MV;
}

std::string MetricContingency::getName() const {
   return "Contingency";
}
