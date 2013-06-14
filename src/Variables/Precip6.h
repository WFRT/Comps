#ifndef VARIABLE_PRECIP6_H
#define VARIABLE_PRECIP6_H
#include "Variable.h"

class VariablePrecip6 : public Variable {
   public:
      VariablePrecip6();
      float computeCore(const Data& iData,
                    int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
};
#endif

