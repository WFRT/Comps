#ifndef REGION_LOCATIONS_H
#define REGION_LOCATIONS_H
#include "Region.h"

//! Create regions at specified locations
class RegionLocations : public Region {
   public:
      RegionLocations(const Options& iOptions, const Data& iData);
   private:
      int findCore(const Location& iLocation) const;
      std::vector<Location> mLocations;
      std::string mDataset;
};
#endif
