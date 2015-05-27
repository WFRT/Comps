#include "Qnh.h"
#include "../Data.h"
#include "../Location.h"

VariableQnh::VariableQnh(const Options& iOptions, const Data& iData) : Variable(iOptions, iData) {
   loadOptionsFromBaseVariable();
   iOptions.check();
}

float VariableQnh::computeCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   float pressure  = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "P");
   float elev = iLocation.getElev();
   if(!Global::isValid(pressure) || !Global::isValid(elev))
      return Global::MV;
   else if(pressure == 0)
      return 0;
   else {
      float g  = 9.80665;   // m/s2
      float T0 = 288.15;    // K
      float L  = 0.0065;    // K/m
      // Method 1:
      // float dElev = 0 - iElev;
      // float M  = 0.0289644; // kg/mol
      // float R  = 8.31447;   // J/(mol•K)
      // float cp = 1007;      // J/(kg•K)
      // float constant = -g*M/R/T0;
      // float qnh = iPressure * pow(1 - L*dElev/T0, g*M/R/L);

      // Method 2: http://www.hochwarth.com/misc/AviationCalculator.html
      float CRGas = 287.053; // [m^2/(s^2*K)] = [J/(kg*K)]
      float p0    = 101.325;  // kPa
      float qnh   = p0*pow(pow((pressure/p0), (CRGas*L)/g) + (elev*L)/T0, g/(CRGas*L));
      return qnh;
   }
}
