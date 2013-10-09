#include "Deterministic.h"

Deterministic::Deterministic() :
      mValue(Global::MV),
      mDate(Global::MV),
      mInit(Global::MV),
      mOffset(Global::MV),
      mVariable("") {
}
Deterministic::Deterministic(float iValue, int iDate, int iInit, float iOffset, const Location& iLocation, const std::string& iVariable) :
      mValue(iValue),
      mDate(iDate),
      mInit(iInit),
      mOffset(iOffset),
      mLocation(iLocation),
      mVariable(iVariable) {

}
