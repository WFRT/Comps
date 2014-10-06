#include "LandUse.h"

NeighbourhoodLandUse::NeighbourhoodLandUse(const Options& iOptions) : Neighbourhood(iOptions), mNum(Global::MV) {
   // How many nearest neighbours should be searched through?
   iOptions.getRequiredValue("num", mNum);
   iOptions.check();
}

std::vector<Location> NeighbourhoodLandUse::selectCore(const Input* iInput, const Location& iLocation) const {
   std::vector<Location> all;
   iInput->getSurroundingLocations(iLocation, all, mNum);
   int landUse = iLocation.getLandUse();
   if(!Global::isValid(landUse)) {
      return all;
   }

   std::vector<Location> locations;
   for(int i = 0; i < all.size(); i++) {
      if(all[i].getLandUse() == landUse)
         locations.push_back(all[i]);
   }

   return locations;
}
