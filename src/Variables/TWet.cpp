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
   /*
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
  */
   float T  = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "T");
   float Rh = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "RH");
   float P  = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "P"); // kPa
   if(!Global::isValid(T) || !Global::isValid(Rh) || !Global::isValid(P))
      return Global::MV;
   float e  = (Rh/100)*0.611*exp((17.63*T)/(T+243.04));
   if(e < 1e-9) {
      // Extremely low humidity, set a lower bound for the TWet value
      const Variable* var = Variable::get("TWet");
      if(var == NULL)
         return Global::MV;
      return var->getMin();
   }
   float Td = (116.9 + 243.04*log(e))/(16.78-log(e));
   float gamma = 0.00066 * P;
   float delta = (4098*e)/pow(Td+243.04,2);
   float TWet   = (gamma * T + delta * Td)/(gamma + delta);
   if(!Global::isValid(e) || !Global::isValid(Td) || !Global::isValid(gamma) || !Global::isValid(delta) || !Global::isValid(TWet)) {
      std::stringstream ss;
      ss << "Invalid TWet values (T, Rh, P, e, Td, gamma, delta, TWet): " << T << " " << Rh << " " << P << " " << e << " " << Td << " " << gamma << " " << delta << " " << TWet << std::endl;
      Global::logger->write(ss.str(), Logger::critical);
   }
   return TWet;
}
