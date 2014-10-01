#ifndef OBS_H
#define OBS_H
#include "Global.h"
#include "Location.h"

class Obs {
   public:
      // Added code here
      Obs();
      Obs(float iValue,
          int iDate,
          float iOffset,
          const std::string& iVariable,
          const Location& iLocation,
          float iError = 0);
      float       getValue() const;
      int         getDate() const;
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

