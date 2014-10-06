#include "Intersection.h"

NeighbourhoodIntersection::NeighbourhoodIntersection(const Options& iOptions) : Neighbourhood(iOptions) {
   //! How many nearest neighbours should be used?
   std::vector<std::string> tags;
   iOptions.getRequiredValues("tags", tags);
   for(int i = 0; i < tags.size(); i++) {
      Neighbourhood* neighbourhood = Neighbourhood::getScheme(tags[i]);
      mNeighbourhoods.push_back(neighbourhood);
   }
   iOptions.check();
}

NeighbourhoodIntersection::~NeighbourhoodIntersection() {
   for(int i = 0; i < mNeighbourhoods.size(); i++) {
      delete mNeighbourhoods[i];
   }
}

std::vector<Location> NeighbourhoodIntersection::selectCore(const Input* iInput, const Location& iLocation) const {
   std::vector<Location> intersection = mNeighbourhoods[0]->select(iInput, iLocation);

   // Loop over remaining neighbourhoods
   for(int n = 1; n < mNeighbourhoods.size(); n++) {
      std::vector<Location> locations = mNeighbourhoods[n]->select(iInput, iLocation);
      std::vector<Location> newIntersection;
      // Only keep locations in 'intersection' that also exist in the new neighbourhood
      for(int j = 0; j < intersection.size(); j++) {
         int currId = intersection[j].getId();
         // Check that this id is in the new neighbourhood
         for(int i = 0; i < locations.size(); i++) {
            if(currId == locations[i].getId()) {
               newIntersection.push_back(intersection[j]);
            }
         }
      }
      intersection = newIntersection;
   }
   return intersection;
}
