#include "NearestNeighbour.h"
#include "../Field.h"
#include "../Data.h"
#include "../Location.h"
DownscalerNearestNeighbour::DownscalerNearestNeighbour(const Options& iOptions, const Data& iData) : Downscaler(iOptions, iData) {}

float DownscalerNearestNeighbour::downscale(const Field& iField,
      const std::string& iVariable,
      const Location& iLocation,
      const Parameters& iParameters) const {

   std::string sliceDataset = iField.getMember().getDataset();
   Input* input = mData.getInput(sliceDataset); //mData.getInput(iField.getMember().getDataset());

   Location useLocation;
   if(iLocation.getDataset() == sliceDataset) {
      useLocation = iLocation;
   }
   else {
      std::vector<Location> locations;
      input->getSurroundingLocations(iLocation, locations);
      useLocation = locations[0];
   }
   std::stringstream ss;
   ss << "Nearest neighbour: " << useLocation.getId() << " " << useLocation.getLat() << " " << useLocation.getLon() << std::endl;
   Global::logger->write(ss.str(), Logger::debug);
   float value = mData.getValue(iField.getDate(), iField.getInit(), iField.getOffset(),
         useLocation, iField.getMember(), iVariable);
   return value;
}
