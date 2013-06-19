#include "Value.h"

Value::Value(float iValue, int iDate, float iOffset, const Location& iLocation, const std::string& iVariable, Input::Type iType) :
      value(iValue),
      date(iDate),
      offset(iOffset),
      location(iLocation),
      variable(iVariable),
      type(iType) {

}
