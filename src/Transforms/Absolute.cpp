#include "Absolute.h"

TransformAbsolute::TransformAbsolute(const Options& iOptions, const Data& iData) : Transform(iOptions, iData)
{}
float TransformAbsolute::transformCore(float iValue) const {
   if(!Global::isValid(iValue))
      return Global::MV;
   if(iValue < 0)
      return -iValue;
   return iValue;
}

float TransformAbsolute::inverseCore(float iValue) const {
   if(!Global::isValid(iValue))
      return Global::MV;
   if(iValue < 0)
      return Global::MV;
   return iValue;
}
