#ifndef POOLER_LOCATIONS_H
#define POOLER_LOCATIONS_H
#include "Pooler.h"

//! Create one pooling index for each obs-locations. Uses nearest neighbour.
class PoolerLocations : public Pooler {
   public:
      PoolerLocations(const Options& iOptions, const Data& iData);
   private:
      int findCore(const Location& iLocation) const;
      std::vector<Location> mLocations;
      std::string mDataset;
};
#endif
