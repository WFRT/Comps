#ifndef VARIABLE_T_WET_H
#define VARIABLE_T_WET_H
#include "Variable.h"

class VariableTWet : public Variable {
   public:
      VariableTWet(const Options& iOptions, const Data& iData);
      float computeCore(int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
      std::string getBaseVariable() const {return "TWet";};
};
#endif

