#ifndef POOLER_ELEVATION_H
#define POOLER_ELEVATION_H
#include "Pooler.h"

//! Create one pooling index for each elevation band.
class PoolerElevation : public Pooler {
   public:
      PoolerElevation(const Options& iOptions, const Data& iData);
   private:
      int findCore(const Location& iLocation) const;
      float mBandwidth;
};
#endif
