#include "TDew.h"
#include "../Data.h"

VariableTDew::VariableTDew() : Variable("TDew") {}

float VariableTDew::computeCore(const Data& iData,
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
   float es = 0.611*exp(5423.0*(1/273.15 - 1/(T+273.15)));
   float e  = Rh/100*(es);
   float Td = 1/(1/273.15 - 1.844e-4*log(e/0.611)) - 273.15;
   //std::cout << iType << " VariableTDew: T = " << T << " Rh = " << Rh << " Td = " << Td << std::endl;
   return Td;
}
