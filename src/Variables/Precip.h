#ifndef VARIABLE_PRECIP_H
#define VARIABLE_PRECIP_H
#include "Variable.h"

class VariablePrecip : public Variable {
   public:
      VariablePrecip();
      float computeCore(const Data& iData,
                    int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
      std::string getBaseVariable() const {return "Precip";};
};
#endif

