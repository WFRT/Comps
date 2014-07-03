#include "WindSpeed.h"
#include "../Data.h"

VariableWindSpeed::VariableWindSpeed(const Options& iOptions, const Data& iData) : Variable(iOptions, iData) {
   loadOptionsFromBaseVariable();
}

float VariableWindSpeed::computeCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   float U  = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "U");
   float V  = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "V");
   if(!Global::isValid(U) || !Global::isValid(V))
      return Global::MV;
   float WindSpeed = std::sqrt(U*U + V*V);
   return WindSpeed;
}
