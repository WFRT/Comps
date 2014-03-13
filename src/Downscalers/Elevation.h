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
      //! Compute temperature after moving a parcel up or down
      float moveParcel(float iTemperatureStart, float iZStart, float iZEnd, float iLapseRate) const;
   private:
      int mNumPoints;
      float mLapseRate; // degrees/km
      bool mComputeLapseRate;
};
#endif
