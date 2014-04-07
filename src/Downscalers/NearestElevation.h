#ifndef DOWNSCALER_NEAREST_ELEVATION_H
#define DOWNSCALER_NEAREST_ELEVATION_H
#include "Downscaler.h"

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
      //! Find which location to use to retrive value for
      Location getBestLocation(const Input* iInput, const Location& iLocation) const;
};
#endif
