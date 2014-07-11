#include "Phase.h"
#include "../Data.h"
#include "../Member.h"

VariablePhase::VariablePhase(const Options& iOptions, const Data& iData) : Variable(iOptions, iData),
      mSnowLimit(0.5),
      mRainLimit(1.5),
      mTemperatureVariable("T"),
      mMinPrecip(0.1),
      mUseModelPhase(false) {
   //! Maximum temperature for which to produce snow (or maximum rain fraction when useModelPhase=1)
   iOptions.getValue("snowLimit", mSnowLimit);
   //! Minimum temperature for which to produce rain (or maximum rain fraction when useModelPhase=1)
   iOptions.getValue("rainLimit", mRainLimit);
   //! Which variable should be used as temperature?
   iOptions.getValue("temperatureVariable", mTemperatureVariable);
   //! Minimum precip amount needed to treat as precip
   iOptions.getValue("minPrecip", mMinPrecip);
   //! Should the precip phase from the model be used?
   iOptions.getValue("useModelPhase", mUseModelPhase);

   loadOptionsFromBaseVariable();
}

float VariablePhase::computeCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {

   if(mUseModelPhase) {
      float Total = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "Precip");
      float Solid = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, "PrecipSolid");
      if(!Global::isValid(Total) || !Global::isValid(Solid))
         return Global::MV;
      if(Total < mMinPrecip)
         return typeNone;

      float Rain  = Total - Solid;
      float fracRain  = Rain / Total;
      if(fracRain <= mSnowLimit)
         return typeSnow;
      if(fracRain >= mRainLimit)
         return typeRain;
      else
         return typeSleet;
   }

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
