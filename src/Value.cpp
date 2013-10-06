#include "Value.h"

Value::Value() :
      mValue(Global::MV),
      mDate(Global::MV),
      mInit(Global::MV),
      mOffset(Global::MV),
      mVariable(""),
      mType(Input::typeForecast) {
}
Value::Value(float iValue, int iDate, int iInit, float iOffset, const Location& iLocation, const std::string& iVariable, Input::Type iType) :
      mValue(iValue),
      mDate(iDate),
      mInit(iInit),
      mOffset(iOffset),
      mLocation(iLocation),
      mVariable(iVariable),
      mType(iType) {

}
