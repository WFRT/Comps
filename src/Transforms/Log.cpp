#include "Log.h"

Log::Log(const Options& iOptions, const Data& iData) : Transform(iOptions, iData),
      mBase(exp(1)) {
   iOptions.getValue("base", mBase);
}
float Log::transform(float iValue) const {
   return log(iValue)/log(mBase);
}

float Log::inverse(float iValue) const {
   return pow(mBase, iValue);
}
