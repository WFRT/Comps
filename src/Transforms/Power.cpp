#include "Power.h"

TransformPower::TransformPower(const Options& iOptions, const Data& iData) : Transform(iOptions, iData) {
   iOptions.getRequiredValue("power", mPower);
}
float TransformPower::transformCore(float iValue) const {
   return pow(iValue, mPower);
}

float TransformPower::inverseCore(float iValue) const {
   return pow(iValue, 1.0/mPower);
}
float TransformPower::derivativeCore(float iValue) const {
   return mPower * pow(iValue,mPower-1);
}
