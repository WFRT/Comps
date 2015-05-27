#ifndef VARIABLE_QNH_H
#define VARIABLE_QNH_H
#include "Variable.h"

class VariableQnh : public Variable {
   public:
      VariableQnh(const Options& iOptions, const Data& iData);
      float computeCore(int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
      std::string getBaseVariable() const {return "QNH";};
};
#endif
