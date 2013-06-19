#ifndef VALUE_H
#define VALUE_H
#include "Global.h"
#include "Inputs/Input.h"
#include "Location.h"

class Value {
   public:
      Value(float iValue, int iDate, float iOffset, const Location& iLocation, const std::string& iVariable, Input::Type iType=Input::typeForecast);
      float value;
      int date;
      float offset;
      Location location;
      std::string variable;
      Input::Type type;
};
#endif
