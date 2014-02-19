#ifndef LOCATION_SELECTOR_NEIGHBOUR_H
#define LOCATION_SELECTOR_NEIGHBOUR_H
#include "LocationSelector.h"

//! Finds the nearest neighbours
class LocationSelectorNeighbour : public LocationSelector {
   public:
      LocationSelectorNeighbour(const Options& iOptions);
   private:
      void selectCore(const Input* iInput, const Location& iLocation, std::vector<Location>& iLocations) const;
      int mNum;
};
#endif

