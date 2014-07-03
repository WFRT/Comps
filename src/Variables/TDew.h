#ifndef VARIABLE_T_DEW_H
#define VARIABLE_T_DEW_H
#include "Variable.h"

class VariableTDew : public Variable {
   public:
      VariableTDew(const Options& iOptions, const Data& iData);
      float computeCore(int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
      std::string getBaseVariable() const {return "TDew";};
};
#endif

