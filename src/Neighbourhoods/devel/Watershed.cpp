#include "Watershed.h"

NeighbourhoodWatershed::NeighbourhoodWatershed(const Options& iOptions) : Neighbourhood(iOptions) {
   Component::underDevelopment();
   //! How many nearest neighbours should be used?
   iOptions.getRequiredValue("num", mNum);
}

void NeighbourhoodWatershed::selectCore(const Input* iInput, const Location& iLocation, std::vector<Location>& iNeighbourhoods) const {
   iInput->getSurroundingLocations(iLocation, iNeighbourhoods, mNum);
   for(int i = 0; i < iNeighbourhood.size(); i++) {
      
   }
}
