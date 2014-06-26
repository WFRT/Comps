#ifndef DOWNSCALER_NEIGHBOURHOOD_H
#define DOWNSCALER_NEIGHBOURHOOD_H
#include "Downscaler.h"
#include "../Neighbourhoods/Neighbourhood.h"

//! Uses the mean of gridpoints within a neighbourhood.
class DownscalerNeighbourhood : public Downscaler {
   public:
      DownscalerNeighbourhood(const Options& iOptions);
      ~DownscalerNeighbourhood();
      float downscale(const Input* iInput,
            int iDate, int iInit, float iOffset,
            const Location& iLocation,
            int iMemberId,
            const std::string& iVariable) const;
   private:
      std::vector<Neighbourhood*> mNeighbourhoods;
      int mWeightOrder;
};
#endif
