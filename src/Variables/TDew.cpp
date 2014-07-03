#include "TDew.h"
#include "../Data.h"

VariableTDew::VariableTDew(const Options& iOptions, const Data& iData) : Variable(iOptions, iData) {
   loadOptionsFromBaseVariable();
}

float VariableTDew::computeCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   float T  = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "T");
   float Rh = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "RH");
   if(!Global::isValid(T) || !Global::isValid(Rh))
      return Global::MV;
   float es = 0.611*exp(5423.0*(1/273.15 - 1/(T+273.15)));
   float e  = Rh/100*(es);
   float Td = 1/(1/273.15 - 1.844e-4*log(e/0.611)) - 273.15;
   return Td;
}
