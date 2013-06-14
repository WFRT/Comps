#include "Distance.h"
#include "../Global.h"
#include "../Options.h"
#include "../Location.h"

FinderDistance::FinderDistance(const Options& iOptions, const Data& iData) : Finder(iOptions, iData) {
   iOptions.getRequiredValues("lats", mLats);
   iOptions.getRequiredValues("lons", mLons);
   assert(mLats.size() == mLons.size());
   mSize = (int) mLats.size();
}

int FinderDistance::findCore(const Location& iLocation) const {
   assert(mLats.size() > 0);
   double minDist = Global::INF;
   int minI = Global::MV;
   for(int i = 0; i < (int) mLats.size(); i++) {
      double dist = Location::getDistance(iLocation.getLat(), iLocation.getLon(), mLats[i], mLons[i]);
      if(dist < minDist) {
         minDist = dist;
         minI = i;
      }
   }
   //std::cout << "Nearest: " << minI << std::endl;
   assert(minI != Global::MV);
   return minI;
}
