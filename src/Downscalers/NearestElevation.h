#ifndef DOWNSCALER_NEAREST_ELEVATION_H
#define DOWNSCALER_NEAREST_ELEVATION_H
#include "../Global.h"
#include "Downscaler.h"
#include "../Cache.h"

//! Use the location with the most similar elevation (within some search radius)
//! Only check the elevations of points within some search radius. If the nearest true neighbour is
//! close enough in elevation, use it.
class DownscalerNearestElevation : public Downscaler {
   public:
      DownscalerNearestElevation(const Options& iOptions);
      float downscale(const Input* iInput,
            int iDate, int iInit, float iOffset,
            const Location& iLocation,
            int iMemberId,
            const std::string& iVariable) const;
   private:
      float mSearchRadius;
      int   mNumNeighbours;
      float mMinElevDiff;
      int   mAlgorithm;
      float mDistanceWeight;
      float mElevationWeight;
      float mGradientWeight;
      int   mNumBest;
      //! Find which location to use to retrive value for
      std::vector<Location> getBestLocations(const Input* iInput, const Location& iLocation) const;

      // Cache of the best lookup locations
      // Key: dataset to find neighbours in, dataset of lookup location, lookup location id -> locations
      // Doesn't seem to give a large speedup
      mutable Cache<Key::Three<std::string,std::string,int>, std::vector<Location> > mBestLocationsCache;
};
#endif
