#ifndef VARIABLE_DEACCUMULATE_H
#define VARIABLE_DEACCUMULATE_H
#include "Variable.h"

//! Deaccumulates a variable over the last N hours.
//! If the current offset is less than N, then accumulate from 0.
class VariableDeaccumulate : public Variable {
   public:
      VariableDeaccumulate(const Options& iOptions, const Data& iData);
      std::string getBaseVariable() const;
   private:
      std::string mBaseVariable;
      float mTimeWindow;
      float computeCore(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            const Member& iMember,
            Input::Type iType = Input::typeUnspecified) const;
      float mLowerLimit;
      float mUpperLimit;
};
#endif

