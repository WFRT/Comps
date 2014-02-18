#ifndef DOWNSCALER_NEAREST_NEIGHBOUR_H
#define DOWNSCALER_NEAREST_NEIGHBOUR_H
#include "Downscaler.h"

//! Use the forecast location nearest the desired output location. Curved earth distances are
//! calculated.
class DownscalerNearestNeighbour : public Downscaler {
   public:
      DownscalerNearestNeighbour(const Options& iOptions);
      float downscale(const Input* iInput,
            int iDate, int iInit, float iOffset,
            const Location& iLocation,
            int iMemberId,
            const std::string& iVariable) const;
};
#endif
