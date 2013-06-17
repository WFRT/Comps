#ifndef DOWNSCALER_NEAREST_NEIGHBOUR_H
#define DOWNSCALER_NEAREST_NEIGHBOUR_H
#include "Downscaler.h"

//! Use the forecast location nearest the desired output location. Curved earth distances are
//! calculated.
class DownscalerNearestNeighbour : public Downscaler {
   public:
      DownscalerNearestNeighbour(const Options& iOptions, const Data& iData);
      float downscale(const Slice& iSlice,
            const std::string& iVariable,
            const Location& iLocation,
            const Parameters& iParameters) const;
      void updateParameters(const Slice& iSlice,
            const std::string& iVariable,
            const Location& iLocation,
            const Obs& iObs,
            Parameters& iParameters) const {};
      bool needsTraining() const {return false;};
};
#endif
