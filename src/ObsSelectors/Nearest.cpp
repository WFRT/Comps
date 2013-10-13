#include "Nearest.h"

ObsSelectorNearest::ObsSelectorNearest(const Options& iOptions, const Data& iData) : ObsSelector(iOptions, iData) {
}

void ObsSelectorNearest::select(std::vector<Obs>& iObs, const Location& iLocation, float iOffset) const {
   float minDistance = Global::MV;
   int   index = Global::MV;
   for(int i = 0; i < iObs.size(); i++) {
      float distance = iLocation.getDistance(iObs[i].getLocation());
      if(!Global::isValid(minDistance) || distance < minDistance) {
         minDistance = distance;
         index = i;
      }
   }
   if(Global::isValid(index)) {
      Obs obs = iObs[index];
      iObs.clear();
      iObs.push_back(obs);
   }
   else {
      iObs.clear();
   }
}
