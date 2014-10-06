#include "LandFraction.h"

NeighbourhoodLandFraction::NeighbourhoodLandFraction(const Options& iOptions) : Neighbourhood(iOptions),
      mNum(Global::MV),
      mTol(0) {
   //! How many nearest neighbours should be searched through?
   iOptions.getRequiredValue("num", mNum);
   //! How different can the fraction be (between 0 and 1)?
   iOptions.getValue("tol", mTol);
   iOptions.check();
}

std::vector<Location> NeighbourhoodLandFraction::selectCore(const Input* iInput, const Location& iLocation) const {
   std::vector<Location> all;
   iInput->getSurroundingLocations(iLocation, all, mNum);
   int landFraction = iLocation.getLandFraction();
   if(!Global::isValid(landFraction)) {
      return all;
   }

   std::vector<Location> locations;
   for(int i = 0; i < all.size(); i++) {
      if(all[i].getLandFraction() == landFraction)
         locations.push_back(all[i]);
   }
   return locations;
}
