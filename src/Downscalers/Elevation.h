#ifndef DOWNSCALER_ELEVATION_H
#define DOWNSCALER_ELEVATION_H
#include "Downscaler.h"

//! Move air parcel dry-adiabatically up/down to desired location, from nearest neighbours.
class DownscalerElevation : public Downscaler {
   public:
      DownscalerElevation(const Options& iOptions, const Data& iData);
      float downscale(const Slice& iSlice,
            const std::string& iVariable,
            const Location& iLocation,
            const Parameters& iParameters) const;
      bool needsTraining() const {return false;};
      //! Compute temperature after following a dry adiabat
      float followDryAdiabat(float iTemperatureStart, float iZStart, float iZEnd) const;
   private:
      int mNumPoints;
      float mLapseRate; // degrees/km
};
#endif
