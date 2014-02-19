#ifndef LOCATION_SELECTOR_WATERSHED_H
#define LOCATION_SELECTOR_WATERSHED_H
#include "LocationSelector.h"

//! Finds nearby locations that are in the same watershed
class LocationSelectorWatershed : public LocationSelector {
   public:
      LocationSelectorWatershed(const Options& iOptions);
   private:
      void selectCore(const Input* iInput, const Location& iLocation, std::vector<Location>& iLocations) const;
      int mNum;
};
#endif

