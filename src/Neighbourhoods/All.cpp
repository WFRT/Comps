#include "All.h"

NeighbourhoodAll::NeighbourhoodAll(const Options& iOptions) : Neighbourhood(iOptions) {
   iOptions.check();
}

std::vector<Location> NeighbourhoodAll::selectCore(const Input* iInput, const Location& iLocation) const {
   return iInput->getLocations();
}
