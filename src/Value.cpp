#include "Value.h"

Value::Value() :
      mValue(Global::MV),
      mType(Input::typeForecast) {
}
Value::Value(float iValue, int iDate, int iInit, float iOffset,
             const Location& iLocation, const std::string& iVariable, Input::Type iType) :
      Entity(iDate, iInit, iOffset, iLocation, iVariable),
      mValue(iValue),
      mType(iType) {

}
