#ifndef NEIGHBOURHOOD_ELEVATION_H
#define NEIGHBOURHOOD_ELEVATION_H
#include "Neighbourhood.h"

//! Create a neighbourhood pf locations that are at a similar elevation
class NeighbourhoodElevation : public Neighbourhood {
   public:
      NeighbourhoodElevation(const Options& iOptions);
   private:
      std::vector<Location> selectCore(const Input* iInput, const Location& iLocation) const;
      float mSearchRadius;
      int   mNum;
      float mMinElevDiff;
      int   mAlgorithm;
      float mDistanceWeight;
      float mElevationWeight;
      float mGradientWeight;
      int   mNumBest;
};
#endif
