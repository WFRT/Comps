#ifndef DETERMINISTIC_H
#define DETERMINISTIC_H
#include "Global.h"
#include "Location.h"
#include "Entity.h"

class Deterministic : public Entity {
   public:
      Deterministic();
      Deterministic(float iValue, int iDate, int iInit, float iOffset, const Location& iLocation, const std::string& iVariable);
      int  getValue() const {return mValue;};
   private:
      float mValue;
};
#endif
