#ifndef OBS_H
#define OBS_H
#include "Global.h"
#include "Location.h"
#include "Entity.h"

class Obs : public Entity {
   public:
      Obs();
      Obs(float iValue,
          int iDate,
          float iOffset,
          const std::string& iVariable,
          const Location& iLocation,
          float iError = 0);
      float       getValue() const;
      int         getDate() const;
      int         getInit() const {return 0;};
      float       getOffset() const;
      std::string getVariable() const;
      Location    getLocation() const;
      float       getError() const;
   private:
      float       mValue;
      int         mDate;
      float       mOffset;
      std::string mVariable;
      Location    mLocation;
      float       mError;
};
#endif

