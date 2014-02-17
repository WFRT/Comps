#ifndef TRANSFORM_ABSOLUTE_H
#define TRANSFORM_ABSOLUTE_H
#include "Transform.h"

//! Gives the absolute value (i.e. turns -5 into +5)
class TransformAbsolute : public Transform {
   public:
      TransformAbsolute(const Options& iOptions);
   protected:
      float transformCore(float iValue) const;
      float inverseCore(float iValue) const;
      float derivativeCore(float iValue) const;
};
#endif
