#ifndef VARIABLE_DEFAULT_H
#define VARIABLE_DEFAULT_H
#include "Variable.h"

class VariableDefault : public Variable {
   public:
      VariableDefault(const Options& iOptions, const Data& iData);
      bool isDerived() const {return false;};
   protected:
      float computeCore(int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
};
#endif
