#ifndef TRANSFORM_LOG_H
#define TRANSFORM_LOG_H
#include "Transform.h"

class TransformLog : public Transform {
   public:
      TransformLog(const Options& iOptions);
   protected:
      float transformCore(float iValue) const;
      float inverseCore(float iValue) const;
      float derivativeCore(float iValue) const;
      float mBase;
};
#endif
