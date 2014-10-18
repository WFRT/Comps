#ifndef NEIGHBOURHOOD_LAND_USE_H
#define NEIGHBOURHOOD_LAND_USE_H
#include "Neighbourhood.h"

//! Create a neighbourhood of locations that are have the same land use category
class NeighbourhoodLandUse : public Neighbourhood {
   public:
      NeighbourhoodLandUse(const Options& iOptions);
   private:
      std::vector<Location> selectCore(const Input* iInput, const Location& iLocation) const;
      int mNum;
};
#endif
