#ifndef VARIABLE_WINDSPEED_H
#define VARIABLE_WINDSPEED_H
#include "Variable.h"

class VariableWindSpeed : public Variable {
   public:
      VariableWindSpeed();
      float computeCore(const Data& iData,
                    int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
};
#endif

