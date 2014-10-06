#include "Union.h"

NeighbourhoodUnion::NeighbourhoodUnion(const Options& iOptions) : Neighbourhood(iOptions) {
   //! How many nearest neighbours should be used?
   std::vector<std::string> tags;
   iOptions.getRequiredValues("tags", tags);
   for(int i = 0; i < tags.size(); i++) {
      Neighbourhood* neighbourhood = Neighbourhood::getScheme(tags[i]);
      mNeighbourhoods.push_back(neighbourhood);
   }
   iOptions.check();
}

NeighbourhoodUnion::~NeighbourhoodUnion() {
   for(int i = 0; i < mNeighbourhoods.size(); i++) {
      delete mNeighbourhoods[i];
   }
}

std::vector<Location> NeighbourhoodUnion::selectCore(const Input* iInput, const Location& iLocation) const {
   std::vector<Location> neighbourhood;

   for(int i = 0; i < mNeighbourhoods.size(); i++) {
      // TODO: Check for duplicates
      std::vector<Location> locations = mNeighbourhoods[i]->select(iInput, iLocation);
      neighbourhood.insert(neighbourhood.end(), locations.begin(), locations.end());
   }
   return neighbourhood;
}
