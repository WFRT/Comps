#ifndef DOWNSCALER_DISTANCE_H
#define DOWNSCALER_DISTANCE_H
#include "Downscaler.h"

//! Use a weighted average, based on distance, of the nearest neighbours
class DownscalerDistance : public Downscaler {
   public:
      DownscalerDistance(const Options& iOptions, const Data& iData);
      float downscale(const Field& iField,
            const std::string& iVariable,
            const Location& iLocation,
            const Parameters& iParameters) const;
      bool needsTraining() const {return false;};
   private:
      int mNumPoints;
      int mOrder;
};
#endif
