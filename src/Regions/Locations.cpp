#include "Locations.h"
#include "../Global.h"
#include "../Options.h"
#include "../Location.h"
#include "../Inputs/Input.h"
#include "../Data.h"
#include "../Location.h"

RegionLocations::RegionLocations(const Options& iOptions, const Data& iData) :
      Region(iOptions, iData),
      mDataset("") {
   if(iOptions.getValue("dataset", mDataset)) {
      Input* input = iData.getInput();
      assert(input != NULL);
      if(input == NULL) {
         std::stringstream ss;
         ss << "RegionLocations: Dataset " << mDataset << " does not exist";
         Global::logger->write(ss.str(), Logger::error);
      }
      mLocations = input->getLocations();
   }
   else {
      std::vector<float> lats;
      std::vector<float> lons;
      iOptions.getRequiredValues("lats", lats);
      iOptions.getRequiredValues("lons", lons);
      assert(lats.size() == lons.size());
      for(int i = 0; i < lats.size(); i++) {
         int id = i;
         float lat = lats[i];
         float lon = lons[i];
         std::string dataset = "";
         Location location(dataset, id, lat, lon);
         mLocations.push_back(location);
      }
   }
   if(mLocations.size() == 0) {
      std::stringstream ss;
      ss << "RegionLocations: No locations available";
      Global::logger->write(ss.str(), Logger::warning);
   }
   //Region::setSize(mLocations.size());
}

int RegionLocations::findCore(const Location& iLocation) const {
   if(iLocation.getDataset() == mDataset)
      return iLocation.getId();

   double minDist = Global::INF;
   int minI = Global::MV;
   for(int i = 0; i < mLocations.size(); i++) {
      double dist = mLocations[i].getDistance(iLocation);
      if(dist < minDist) {
         minDist = dist;
         minI = i;
      }
   }
   return minI;
}
