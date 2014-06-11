#include "Watershed.h"

LocationSelectorWatershed::LocationSelectorWatershed(const Options& iOptions) : LocationSelector(iOptions) {
   Component::underDevelopment();
   //! How many nearest neighbours should be used?
   iOptions.getRequiredValue("num", mNum);
}

void LocationSelectorWatershed::selectCore(const Input* iInput, const Location& iLocation, std::vector<Location>& iLocationSelectors) const {
   iInput->getSurroundingLocations(iLocation, iLocationSelectors, mNum);
   for(int i = 0; i < iLocationSelector.size(); i++) {
      
   }
}
