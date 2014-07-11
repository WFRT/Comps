#include "TWet.h"
#include "../Data.h"

VariableTWet::VariableTWet(const Options& iOptions, const Data& iData) : Variable(iOptions, iData) {
   loadOptionsFromBaseVariable();
}

float VariableTWet::computeCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   float T  = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "T");
   float Rh = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "RH");
   if(!Global::isValid(T) || !Global::isValid(Rh))
      return Global::MV;
   float TWet = T * atan(0.15197*pow(Rh + 8.313659,0.5))
              + atan(T + Rh) - atan(Rh - 1.676331)
              + 0.00391838*pow(Rh,1.5)*atan(0.023101*Rh)
              - 4.686035;
   // std::cout << iOffset << " " << iType << " " << T << " " << Rh << " " << TWet << std::endl;
   //std::cout << iType << " VariableTWet: T = " << T << " Rh = " << Rh << " Td = " << Td << std::endl;

   /*
      float RH = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "RH");
      float TK = T + 273.15;
      float e  = (RH/100)*0.611*exp((17.63*TK)/(TK+243.04));
      float Td = (116.9 + 243.04*log(e))/(16.78-log(e));
      float P  = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "P"); // kPa
      float gamma = 0.00066 * P;
      float delta = (4098*e)/pow(Td+243.04,2);
      float TwK   = (gamma * TK + delta * Td)/(gamma + delta);
      T = TwK - 273.15;
   */
   return TWet;
}
