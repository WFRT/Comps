#include "Power.h"
#include "../Data.h"

VariablePower::VariablePower(const Options& iOptions, const Data& iData) : Variable(iOptions, iData) {
   iOptions.getRequiredValue("turbineRadius", mTurbineRadius);
   iOptions.getRequiredValue("lowerCutOffSpeed", mLowerCutoff);
   iOptions.getRequiredValue("upperCutOffSpeed", mUpperCutoff);

   loadOptionsFromBaseVariable();
}

float VariablePower::computeCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   float WS  = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "WS");
   if(!Global::isValid(WS))
      return Global::MV;
   if(WS < mLowerCutoff || WS > mUpperCutoff)
      return 0;
   float Power = std::pow(WS, 3) * mTurbineRadius*mTurbineRadius;
   return Power;
}
