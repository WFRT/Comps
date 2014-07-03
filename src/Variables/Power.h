#ifndef VARIABLE_POWER_H
#define VARIABLE_POWER_H
#include "Variable.h"

class VariablePower : public Variable {
   public:
      VariablePower(const Options& iOptions, const Data& iData);
      float computeCore(int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
      std::string getBaseVariable() const {return "Power";};
   private:
      float mTurbineRadius; // 
      float mLowerCutoff; // Speed (m/s) below which no power is generated
      float mUpperCutoff; // Speed (m/s) above which no power is generated
};
#endif

