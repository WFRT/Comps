#ifndef TRANSFORM_LOG_H
#define TRANSFORM_LOG_H
#include "Transform.h"

class TransformLog : public Transform {
   public:
      TransformLog(const Options& iOptions, const Data& iData);
   protected:
      float transformCore(float iValue) const;
      float inverseCore(float iValue) const;
      float mBase;
};
#endif
