#include "NearestNeighbour.h"
#include "../Field.h"
#include "../Data.h"
#include "../Location.h"
DownscalerNearestNeighbour::DownscalerNearestNeighbour(const Options& iOptions) : Downscaler(iOptions) {}

float DownscalerNearestNeighbour::downscale(const Input* iInput,
      int iDate, int iInit, float iOffset,
      const Location& iLocation,
      int iMemberId,
      const std::string& iVariable) const {

   Location useLocation;
   if(iLocation.getDataset() == iInput->getName()) {
      useLocation = iLocation;
   }
   else {
      std::vector<Location> locations;
      iInput->getSurroundingLocations(iLocation, locations);
      useLocation = locations[0];
   }
   std::stringstream ss;
   ss << "Nearest neighbour: " << useLocation.getId() << " " << useLocation.getLat() << " " << useLocation.getLon() << std::endl;
   Global::logger->write(ss.str(), Logger::debug);
   float value = iInput->getValue(iDate, iInit, iOffset, useLocation.getId(), iMemberId, iVariable);
   return value;
}
