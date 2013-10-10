#ifndef ENTITY_H
#define ENTITY_H
#include <string>
#include "Location.h"

// Abstract class containing date, init, offset, location, and variable
class Entity {
   public:
      Entity();
      Entity(int iDate, int iInit, float iOffset, const Location& iLocation, const std::string& iVariable);
      virtual int  getDate() const {return mDate;};
      virtual int  getInit() const {return mInit;};
      virtual float getOffset() const {return mOffset;};
      virtual Location getLocation() const {return mLocation;};
      virtual std::string getVariable() const {return mVariable;};
   private:
      int mDate;
      int mInit;
      float mOffset;
      Location mLocation;
      std::string mVariable;
};
#endif
