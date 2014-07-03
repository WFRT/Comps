#include "Phase.h"
#include "../Data.h"
#include "../Member.h"

VariablePhase::VariablePhase(const Options& iOptions, const Data& iData) : Variable(iOptions, iData),
      mSnowLimit(0.5),
      mRainLimit(1.5),
      mTemperatureVariable("T"),
      mMinPrecip(0.1) {
   //! Maximum temperature for which to produce snow
   iOptions.getValue("snowLimit", mSnowLimit);
   //! Minimum temperature for which to produce rain
   iOptions.getValue("rainLimit", mRainLimit);
   //! Which variable should be used as temperature?
   iOptions.getValue("temperatureVariable", mTemperatureVariable);
   //! Minimum precip amount needed to treat as precip
   iOptions.getValue("minPrecip", mMinPrecip);

   loadOptionsFromBaseVariable();
}

float VariablePhase::computeCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {

   float T   = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, mTemperatureVariable);
   float Pcp = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "Precip");

   if(!Global::isValid(T) || !Global::isValid(Pcp))
      return Global::MV;
   if(Pcp < mMinPrecip)
      return typeNone;
   if(T <= mSnowLimit)
      return typeSnow;
   if(T >= mRainLimit)
      return typeRain;
   else
      return typeSleet;
}
