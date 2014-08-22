#include "Grid.h"
#include "../Location.h"
#include "../Member.h"
#include "../Options.h"

InputGrid::InputGrid(const Options& iOptions) :
      Input(iOptions) {
   //! Vector of latitudes (in degrees)
   iOptions.getRequiredValues("lats", mLats);
   //! Vector of longitudes (in degrees)
   iOptions.getRequiredValues("lons", mLons);

   iOptions.check();
   init();
}

void InputGrid::getLocationsCore(std::vector<Location>& iLocations) const {
   std::cout << "Getting locations" << std::endl;
   iLocations.clear();
   int id = 0;
   for(int i = 0; i < mLats.size(); i++) {
      float lat  = mLats[i];
      for(int j = 0; j < mLons.size(); j++) {
         float lon  = mLons[j];
         float elev = 0;
         Location loc(getName(), id, lat, lon);
         loc.setElev(elev);
         iLocations.push_back(loc);
         id++;
      }
   }
}
