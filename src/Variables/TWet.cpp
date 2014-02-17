#include "TWet.h"
#include "../Data.h"

VariableTWet::VariableTWet() : Variable("TWet") {}

float VariableTWet::computeCore(const Data& iData,
      int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   float T  = iData.getValue(iDate, iInit, iOffset, iLocation, iMember, "T");
   float Rh = iData.getValue(iDate, iInit, iOffset, iLocation, iMember, "RH");
   if(!Global::isValid(T) || !Global::isValid(Rh))
      return Global::MV;
   float TWet = T * atan(0.15197*pow(Rh + 8.313659,0.5))
              + atan(T + Rh) - atan(Rh - 1.676331)
              + 0.00391838*pow(Rh,1.5)*atan(0.023101*Rh)
              - 4.686035;
   //std::cout << iType << " VariableTWet: T = " << T << " Rh = " << Rh << " Td = " << Td << std::endl;
   return TWet;
}
