#ifndef TRANSFORM_POWER_H
#define TRANSFORM_POWER_H
#include "Transform.h"

class TransformPower : public Transform {
   public:
      TransformPower(const Options& iOptions);
   protected:
      float transformCore(float iValue) const;
      float inverseCore(float iValue) const;
      float derivativeCore(float iValue) const;
      float mPower;
};
#endif
