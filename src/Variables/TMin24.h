#ifndef VARIABLE_TMIN24_H
#define VARIABLE_TMIN24_H
#include "Variable.h"

class VariableTMin24 : public Variable {
   public:
      VariableTMin24();
      float computeCore(const Data& iData,
                    int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
};
#endif

