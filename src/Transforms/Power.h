#ifndef TRANSFORM_POWER_H
#define TRANSFORM_POWER_H
#include "Transform.h"

class Power : public Transform {
   public:
      Power(const Options& iOptions, const Data& iData);
   protected:
      float transform(float iValue) const;
      float inverse(float iValue) const;
      float mPower;
};
#endif
