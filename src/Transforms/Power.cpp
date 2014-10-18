#include "Power.h"

TransformPower::TransformPower(const Options& iOptions) : Transform(iOptions) {
   iOptions.getRequiredValue("power", mPower);
   iOptions.check();
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
