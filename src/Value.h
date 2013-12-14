#ifndef VALUE_H
#define VALUE_H
#include "Global.h"
#include "Inputs/Input.h"
#include "Location.h"
#include "Entity.h"

class Value : public Entity {
   public:
      Value();
      Value(float iValue, int iDate, int iInit, float iOffset, const Location& iLocation, const std::string& iVariable, Input::Type iType=Input::typeForecast);
      float getValue() const {return mValue;};
      Input::Type getType() const {return mType;};
   private:
      float mValue;
      Input::Type mType;
};
#endif
