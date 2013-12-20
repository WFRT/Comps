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
          int iInit,
          float iOffset,
          const std::string& iVariable,
          const Location& iLocation,
          float iError = 0);
      float       getValue() const;
      float       getError() const;
   private:
      float       mValue;
      float       mError;
};
#endif
