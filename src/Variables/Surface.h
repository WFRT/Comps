#ifndef VARIABLE_SURFACE_H
#define VARIABLE_SURFACE_H
#include "Variable.h"

class VariableSurface : public Variable {
   public:
      VariableSurface();
      float computeCore(const Data& iData,
                    int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
};
#endif

