#ifndef TRANSFORM_LOG_H
#define TRANSFORM_LOG_H
#include "Transform.h"

class Log : public Transform {
   public:
      Log(const Options& iOptions, const Data& iData);
   protected:
      float transform(float iValue) const;
      float inverse(float iValue) const;
      float mBase;
};
#endif
