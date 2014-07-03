#ifndef VARIABLE_MAXIMUM_H
#define VARIABLE_MAXIMUM_H
#include "Variable.h"

class VariableMaximum : public Variable {
   public:
      VariableMaximum(const Options& iOptions, const Data& iData);
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
