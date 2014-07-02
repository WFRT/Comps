#ifndef VARIABLE_PRECIP_SOLID_H
#define VARIABLE_PRECIP_SOLID_H
#include "Variable.h"

class VariablePrecipSolid : public Variable {
   public:
      VariablePrecipSolid();
      float computeCore(const Data& iData,
                    int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
};
#endif

