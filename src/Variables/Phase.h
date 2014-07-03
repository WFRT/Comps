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
      std::string getBaseVariable() const {return "Phase";};
      enum Type {typeNone = 0, typeRain = 1, typeSleet = 2, typeSnow = 3};
   private:
      float mSnowLimit;
      float mRainLimit;
      bool  mUseWetBulb;
      float mMinPrecip;
      std::string mTemperatureVariable;
};
#endif

