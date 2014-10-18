#include "Absolute.h"

TransformAbsolute::TransformAbsolute(const Options& iOptions) : Transform(iOptions) {
   iOptions.check();
}
float TransformAbsolute::transformCore(float iValue) const {
   if(iValue < 0)
      return -iValue;
   return iValue;
}

float TransformAbsolute::inverseCore(float iValue) const {
   if(iValue < 0)
      return Global::MV;
   return iValue;
}

float TransformAbsolute::derivativeCore(float iValue) const {
   if(iValue < 0)
      return -1;
   if(iValue == 0)
      return 0;
   return 1;
}
