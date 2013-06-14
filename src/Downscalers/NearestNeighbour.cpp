#include "NearestNeighbour.h"
#include "../Slice.h"
#include "../Data.h"
#include "../Location.h"
DownscalerNearestNeighbour::DownscalerNearestNeighbour(const Options& iOptions, const Data& iData) : Downscaler(iOptions, iData) {}

float DownscalerNearestNeighbour::downscale(const Slice& iSlice,
      const std::string& iVariable,
      const Location& iLocation,
      const Parameters& iParameters) const {

   std::string sliceDataset = iSlice.getMember().getDataset();
   Input* input = mData.getInput(sliceDataset); //mData.getInput(iSlice.getMember().getDataset());

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
   float value = mData.getValue(iSlice.getDate(), iSlice.getInit(), iSlice.getOffset(),
         useLocation, iSlice.getMember(), iVariable);
   return value;
}
