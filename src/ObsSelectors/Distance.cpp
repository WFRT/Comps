#include "Distance.h"

ObsSelectorDistance::ObsSelectorDistance(const Options& iOptions, const Data& iData) : ObsSelector(iOptions, iData) {
   //! Only include observations within this distance in km
   iOptions.getRequiredValue("maxDistance", mMaxDistance);
}

void ObsSelectorDistance::select(std::vector<Obs>& iObs, const Location& iLocation, float iOffset) const {
/*   for(int i = 0; i < iObs.size(); i++) {
      float distance = iLocation.getDistance(iObs[i].getLocation());
      if(distance <= mMaxDistance) {
         iIndices.push_back(i);
      }
   }
   */
}
