#ifndef VARIABLE_LOAD0_H
#define VARIABLE_LOAD0_H
#include "Variable.h"

class VariableLoad0 : public Variable {
   public:
      VariableLoad0();
   private:
      float computeCore(const Data& iData,
                    int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
      static float gepModelL(const std::vector<float>& iValues);
};
#endif

