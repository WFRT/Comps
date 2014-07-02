#ifndef VARIABLE_TMAX24_H
#define VARIABLE_TMAX24_H
#include "Variable.h"

class VariableTMax24 : public Variable {
   public:
      VariableTMax24();
      float computeCore(const Data& iData,
                    int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
};
#endif

