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
      int  getDate() const {return mDate;};
      int  getInit() const {return mInit;};
      float getOffset() const {return mOffset;};
      Location getLocation() const {return mLocation;};
      std::string getVariable() const {return mVariable;};
   private:
      float mValue;
      int mDate;
      int mInit;
      float mOffset;
      Location mLocation;
      std::string mVariable;
};
#endif
