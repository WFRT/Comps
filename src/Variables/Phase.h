#ifndef VARIABLE_PHASE_H
#define VARIABLE_PHASE_H
#include "Variable.h"

class VariablePhase : public Variable {
   public:
      VariablePhase();
      float computeCore(const Data& iData,
                    int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
      enum Type {typeNone = 0, typeRain = 1, typeSleet = 2, typeSnow = 3};
   private:
      float mSleetStart;
      float mSnowStart;
      bool  mUseWetBulb;
};
#endif

