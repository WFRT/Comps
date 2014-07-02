#include "Phase.h"
#include "../Data.h"
#include "../Member.h"

VariablePhase::VariablePhase(const Options& iOptions, const Data& iData) : Variable(iOptions, iData),
      mSleetStart(2),
      mSnowStart(1),
      mUseWetBulb(false),
      mMinPrecip(0.1) {
   iOptions.getValue("sleetStart", mSleetStart);
   iOptions.getValue("snowStart", mSnowStart);
   // Determine phase using the wetbulb temperature, instead of air temperature
   iOptions.getValue("useWetBulb", mUseWetBulb);
   //! Minimum precip amount needed to treat as precip
   iOptions.getValue("minPrecip", mMinPrecip);

   mName = "Phase";
}

float VariablePhase::computeCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   std::string variable = "T";
   if(mUseWetBulb) {
      Input* input = mData.getInput(iMember.getDataset());
      if(input->hasVariable("TWet")) {
         variable = "TWet";
      }
   }

   float T   = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, variable);
   float PCP = 10;//mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "Precip");

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
