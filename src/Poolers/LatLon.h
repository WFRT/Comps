#ifndef POOLER_LAT_LON_H
#define POOLER_LAT_LON_H
#include "Pooler.h"

//! Create one pooling index for each elevation band.
class PoolerLatLon : public Pooler {
   public:
      PoolerLatLon(const Options& iOptions, const Data& iData);
   private:
      int findCore(const Location& iLocation) const;
      std::vector<float> mLatEdges;
      std::vector<float> mLonEdges;
};
#endif
