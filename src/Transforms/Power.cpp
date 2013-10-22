#include "Power.h"

Power::Power(const Options& iOptions, const Data& iData) : Transform(iOptions, iData) {
   iOptions.getRequiredValue("power", mPower);
}
float Power::transform(float iValue) const {
   return pow(iValue, mPower);
}

float Power::inverse(float iValue) const {
   return pow(iValue, 1.0/mPower);
}
