#include "WindDir.h"
#include "../Data.h"

VariableWindDir::VariableWindDir(const Options& iOptions, const Data& iData) : Variable(iOptions, iData) {
   loadOptionsFromBaseVariable();
}

float VariableWindDir::computeCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   float U  = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "U");
   float V  = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "V");
   if(!Global::isValid(U) || !Global::isValid(V))
      return Global::MV;
   if(U == 0 && V == 0)
      return Global::MV;

   float windDir = std::atan2(-U,-V) * 180 / Global::pi;
   if(windDir < 0)
      windDir += 360;
   return windDir;
}
