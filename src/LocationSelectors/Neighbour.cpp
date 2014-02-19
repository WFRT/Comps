#include "Neighbour.h"

LocationSelectorNeighbour::LocationSelectorNeighbour(const Options& iOptions) : LocationSelector(iOptions) {
   //! How many nearest neighbours should be used?
   iOptions.getRequiredValue("num", mNum);
}

void LocationSelectorNeighbour::selectCore(const Input* iInput, const Location& iLocation, std::vector<Location>& iLocationSelectors) const {
   iInput->getSurroundingLocations(iLocation, iLocationSelectors, mNum);
}
