#include "Power.h"
#include "../Data.h"

VariablePower::VariablePower() : Variable("Power") {
   mOptions.getRequiredValue("turbineRadius", mTurbineRadius);
   mOptions.getRequiredValue("lowerCutOffSpeed", mLowerCutoff);
   mOptions.getRequiredValue("upperCutOffSpeed", mUpperCutoff);
}

float VariablePower::computeCore(const Data& iData,
      int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   float WS  = iData.getValue(iDate, iInit, iOffset, iLocation, iMember, "WS");
   if(!Global::isValid(WS))
      return Global::MV;
   if(WS < mLowerCutoff || WS > mUpperCutoff)
      return 0;
   float Power = std::pow(WS, 3) * mTurbineRadius*mTurbineRadius;
   return Power;
}
