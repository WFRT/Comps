#ifndef NEIGHBOURHOOD_LAND_FRACTION_H
#define NEIGHBOURHOOD_LAND_FRACTION_H
#include "Neighbourhood.h"

//! Create a neighbourhood of locations that are have similar land fractions
class NeighbourhoodLandFraction : public Neighbourhood {
   public:
      NeighbourhoodLandFraction(const Options& iOptions);
   private:
      std::vector<Location> selectCore(const Input* iInput, const Location& iLocation) const;
      int mNum;
      float mTol;
};
#endif
