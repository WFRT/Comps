#ifndef VARIABLE_WINDSPEED_H
#define VARIABLE_WINDSPEED_H
#include "Variable.h"

class VariableWindSpeed : public Variable {
   public:
      VariableWindSpeed(const Options& iOptions, const Data& iData);
      float computeCore(int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const;
      std::string getBaseVariable() const {return "WindSpeed";};
};
#endif

