#include "Nearest.h"

NeighbourhoodNearest::NeighbourhoodNearest(const Options& iOptions) : Neighbourhood(iOptions),
      mSkipIdentical(false) {
   //! How many nearest neighbours should be used?
   iOptions.getRequiredValue("num", mNum);

   //! Don't include the nearest neighbour if it happens to be identical to the lookup location
   iOptions.getValue("skipIdentical", mSkipIdentical);

   iOptions.check();
}

std::vector<Location> NeighbourhoodNearest::selectCore(const Input* iInput, const Location& iLocation) const {
   std::vector<Location> neighbourhood;

   if(!mSkipIdentical && iLocation.getDataset() == iInput->getName() && mNum == 1) {
      neighbourhood.push_back(iLocation);
   }
   else {
      if(mSkipIdentical) {
         // Check if the lat/lon/elevation are the same
         float tol = 1e-5;
         float elevTol = 1;
         iInput->getSurroundingLocations(iLocation, neighbourhood, mNum+1);
         if(neighbourhood.size() > 1 &&
            abs(neighbourhood[0].getLat() - iLocation.getLat()) < tol &&
            abs(neighbourhood[0].getLon() - iLocation.getLon()) < tol &&
            abs(neighbourhood[0].getElev() - iLocation.getElev()) < elevTol) {
            neighbourhood.erase(neighbourhood.begin());
         }
         else {
            neighbourhood.pop_back();
         }
      }
      else {
         iInput->getSurroundingLocations(iLocation, neighbourhood, mNum);
      }
   }
   return neighbourhood;
}
