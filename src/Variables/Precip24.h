#ifndef VARIABLE_PRECIP24_H
#define VARIABLE_PRECIP24_H
#include "Variable.h"

class VariablePrecip24 : public Variable {
   public:
      VariablePrecip24();
      float computeCore(const Data& iData,
                    int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
};
#endif

