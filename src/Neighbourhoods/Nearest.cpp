#include "Nearest.h"

NeighbourhoodNearest::NeighbourhoodNearest(const Options& iOptions) : Neighbourhood(iOptions) {
   //! How many nearest neighbours should be used?
   iOptions.getRequiredValue("num", mNum);
}

std::vector<Location> NeighbourhoodNearest::selectCore(const Input* iInput, const Location& iLocation) const {
   std::vector<Location> neighbourhood;

   if(iLocation.getDataset() == iInput->getName() && mNum == 1) {
      neighbourhood.push_back(iLocation);
   }
   else {
      iInput->getSurroundingLocations(iLocation, neighbourhood, mNum);
   }
   return neighbourhood;
}
