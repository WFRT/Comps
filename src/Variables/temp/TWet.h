#ifndef VARIABLE_T_WET_H
#define VARIABLE_T_WET_H
#include "Variable.h"

class VariableTWet : public Variable {
   public:
      VariableTWet();
      float computeCore(const Data& iData,
                    int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
};
#endif

