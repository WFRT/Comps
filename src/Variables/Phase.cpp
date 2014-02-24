#include "Phase.h"
#include "../Data.h"
#include "../Member.h"

VariablePhase::VariablePhase() : Variable("Phase"),
      mSleetStart(2),
      mSnowStart(1),
      mUseWetBulb(false) {
   mOptions.getValue("sleepStart", mSleetStart);
   mOptions.getValue("SnowStart", mSnowStart);
   // Determine phase using the wetbulb temperature, instead of air temperature
   mOptions.getValue("useWetBulb", mUseWetBulb);
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

   float T = iData.getValue(iDate, iInit, iOffset, iLocation, iMember, variable);

   if(!Global::isValid(T))
      return Global::MV;
   if(T < mSnowStart)
      return typeSnow;
   if(T < mSleetStart)
      return typeSleet;
   else
      return typeRain;
}
