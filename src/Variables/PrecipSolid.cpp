#include "PrecipSolid.h"
#include "../Data.h"

VariablePrecipSolid::VariablePrecipSolid() : Variable("PrecipSolid") {}

float VariablePrecipSolid::computeCore(const Data& iData,
      int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   float T   = iData.getValue(iDate, iInit, iOffset, iLocation, iMember, "T");
   float Pcp = iData.getValue(iDate, iInit, iOffset, iLocation, iMember, "Precip");
   if(!Global::isValid(T) || !Global::isValid(Pcp))
      return Global::MV;

   float PrecipSolid;
   if(T >= 0)
      PrecipSolid = 0;
   else
      PrecipSolid = Pcp;
   return PrecipSolid;
}
