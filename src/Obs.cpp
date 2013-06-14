#include "Obs.h"

Obs::Obs() :
   mValue(Global::MV),
   mDate(Global::MV),
   mOffset(Global::MV),
   mVariable(""),
   mError(Global::MV) {};
Obs::Obs(float iValue, int iDate, float iOffset, const std::string& iVariable, const Location& iLocation, float iError) :
   mValue(iValue),
   mDate(iDate),
   mOffset(iOffset),
   mVariable(iVariable),
   mLocation(iLocation),
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
int Obs::getDate() const {
   return mDate;
}
float Obs::getOffset() const {
   return mOffset;
}
std::string Obs::getVariable() const {
   return mVariable;
}
float Obs::getError() const {
   return mError;
}
Location Obs::getLocation() const {
   return mLocation;
}
