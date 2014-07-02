#ifndef VARIABLE_T_DEW_H
#define VARIABLE_T_DEW_H
#include "Variable.h"

class VariableTDew : public Variable {
   public:
      VariableTDew();
      float computeCore(const Data& iData,
                    int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
};
#endif

