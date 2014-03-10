#ifndef DOWNSCALER_ELEVATION_H
#define DOWNSCALER_ELEVATION_H
#include "Downscaler.h"

//! Move air parcel dry-adiabatically up/down to desired location, from nearest neighbours.
class DownscalerElevation : public Downscaler {
   public:
      DownscalerElevation(const Options& iOptions);
      float downscale(const Input* iInput,
            int iDate, int iInit, float iOffset,
            const Location& iLocation,
            int iMemberId,
            const std::string& iVariable) const;
      //! Compute temperature after following a dry adiabat
      float followDryAdiabat(float iTemperatureStart, float iZStart, float iZEnd) const;
   private:
      int mNumPoints;
      float mLapseRate; // degrees/km
      bool mComputeLapseRate;
};
#endif
