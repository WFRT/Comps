#include "Phase.h"
#include "../Data.h"
#include "../Member.h"

VariablePhase::VariablePhase() : Variable("Phase"),
      mSleetStart(2),
      mSnowStart(1),
      mUseWetBulb(false),
      mMinPrecip(0.1) {
   mOptions.getValue("sleetStart", mSleetStart);
   mOptions.getValue("SnowStart", mSnowStart);
   // Determine phase using the wetbulb temperature, instead of air temperature
   mOptions.getValue("useWetBulb", mUseWetBulb);
   //! Minimum precip amount needed to treat as precip
   mOptions.getValue("minPrecip", mMinPrecip);
}

float VariablePhase::computeCore(const Data& iData,
      int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   std::string variable = "T";
   if(mUseWetBulb) {
      Input* input = iData.getInput(iMember.getDataset());
      if(input->hasVariable("TWet")) {
         variable = "TWet";
      }
   }

   float T   = iData.getValue(iDate, iInit, iOffset, iLocation, iMember, variable);
   float PCP = iData.getValue(iDate, iInit, iOffset, iLocation, iMember, "Precip");

   if(!Global::isValid(T) || !Global::isValid(PCP))
      return Global::MV;
   if(PCP < mMinPrecip)
      return typeNone;
   if(T < mSnowStart)
      return typeSnow;
   if(T < mSleetStart)
      return typeSleet;
   else
      return typeRain;
}
