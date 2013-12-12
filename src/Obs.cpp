#include "Obs.h"

Obs::Obs() :
   mValue(Global::MV),
   mError(Global::MV) {};
Obs::Obs(float iValue, int iDate, int iInit, float iOffset, const std::string& iVariable, const Location& iLocation, float iError) :
      Entity(iDate, iInit, iOffset, iLocation, iVariable),
      mValue(iValue),
      mError(iError)
{
   if(mOffset >= 24) {
      mDate = Global::getDate(mDate, mOffset);
      mOffset = fmod(mOffset, 24);
   }
}
float Obs::getValue() const {
   return mValue;
}
float Obs::getError() const {
   return mError;
}
