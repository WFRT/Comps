#ifndef VARIABLE_POWER_H
#define VARIABLE_POWER_H
#include "Variable.h"

class VariablePower : public Variable {
   public:
      VariablePower();
      float computeCore(const Data& iData,
                    int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
   private:
      float mTurbineRadius; // 
      float mLowerCutoff; // Speed (m/s) below which no power is generated
      float mUpperCutoff; // Speed (m/s) above which no power is generated
};
#endif

