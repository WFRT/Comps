#ifndef DOWNSCALER_DISTANCE_H
#define DOWNSCALER_DISTANCE_H
#include "Downscaler.h"

//! Use a weighted average, based on distance, of the nearest neighbours
class DownscalerDistance : public Downscaler {
   public:
      DownscalerDistance(const Options& iOptions);
      float downscale(const Input* iInput,
            int iDate, int iInit, float iOffset,
            const Location& iLocation,
            int iMemberId,
            const std::string& iVariable) const;
   private:
      int mNumPoints;
      int mOrder;
};
#endif
