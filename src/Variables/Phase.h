#ifndef VARIABLE_PHASE_H
#define VARIABLE_PHASE_H
#include "Variable.h"

class VariablePhase : public Variable {
   public:
      VariablePhase(const Options& iOptions, const Data& iData);
      float computeCore(
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
      float mMinPrecip;
};
#endif

