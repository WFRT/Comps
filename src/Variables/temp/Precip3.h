#ifndef VARIABLE_PRECIP3_H
#define VARIABLE_PRECIP3_H
#include "Variable.h"

class VariablePrecip3 : public Variable {
   public:
      VariablePrecip3();
      float computeCore(const Data& iData,
                    int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
      std::string getBaseVariable() const {return "PrecipAcc";};
};
#endif

