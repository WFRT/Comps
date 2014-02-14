#ifndef DOWNSCALER_NEAREST_ELEVATION_H
#define DOWNSCALER_NEAREST_ELEVATION_H
#include "Downscaler.h"

//! Use the location with the most similar elevation (within some search radius)
//! Only check the elevations of points within some search radius. If the nearest true neighbour is
//! close enough in elevation, use it.
class DownscalerNearestElevation : public Downscaler {
   public:
      DownscalerNearestElevation(const Options& iOptions, const Data& iData);
      float downscale(const Field& iField,
            const std::string& iVariable,
            const Location& iLocation,
            const Parameters& iParameters) const;
      bool needsTraining() const {return false;};
   private:
      float mSearchRadius;
      float mMinElevDiff;
      //! Find which location to use to retrive value for
      Location getBestLocation(Input* iInput, const Location& iLocation) const;
};
#endif
