#ifndef VARIABLE_ACCUMULATE_H
#define VARIABLE_ACCUMULATE_H
#include "Variable.h"

//! Accumulates a variable over the last N hours.
//! If the current offset is less than N, then accumulate from 0.
class VariableAccumulate : public Variable {
   public:
      VariableAccumulate(const Options& iOptions, const Data& iData);
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
};
#endif

