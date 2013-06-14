#include "Metric.h"
#include "SchemesHeader.inc"

Metric::Metric(const Options& iOptions, const Data& iData) : Component(iOptions, iData)
      //mDetFlag(Forecaster::detFlagCorrected), 
      //mProbFlag(Forecaster::probFlagCalibrated)
      {
   // TODO:
   //iOptions.getValue("detFlag");
}

#include "Schemes.inc"

/*
std::string Metric::getName() const {
   std::stringstream ss;
   ss << typeid(this).name();
   return ss.str();
}
*/
