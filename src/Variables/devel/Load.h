#ifndef VARIABLE_LOAD_H
#define VARIABLE_LOAD_H
#include "Variable.h"

class VariableLoad : public Variable {
   public:
      VariableLoad();
   private:
      float computeCore(const Data& iData,
                    int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
      static float gepModelBias(const std::vector<float>& iValues);
};
#endif

