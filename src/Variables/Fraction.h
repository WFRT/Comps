#ifndef VARIABLE_FRACTION_H
#define VARIABLE_FRACTION_H
#include "Variable.h"

class VariableFraction : public Variable {
   public:
      VariableFraction(const Options& iOptions, const Data& iData);
      float computeCore(
                    int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
      std::string getBaseVariable() const;
   private:
      std::string mFractionVariable;
      std::string mTotalVariable;
};
#endif

