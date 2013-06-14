#ifndef VARIABLE_BYPASS_H
#define VARIABLE_BYPASS_H
#include "Variable.h"

class VariableBypass : public Variable {
   public:
      VariableBypass(std::string iName);
      std::string getBaseVariable() const {return mName;};
      bool isDerived() const {return false;};
   protected:
      float computeCore(const Data& iData,
                    int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
};
#endif

