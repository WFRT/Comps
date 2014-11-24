#include "LatLon.h"
#include "../Global.h"
#include "../Options.h"
#include "../Location.h"
#include "../Inputs/Input.h"
#include "../Data.h"
#include "../Location.h"

PoolerLatLon::PoolerLatLon(const Options& iOptions, const Data& iData) : Pooler(iOptions, iData) {
   //! What bins should elevation be split into?
   iOptions.getValues("latEdges", mLatEdges);
   iOptions.getValues("lonEdges", mLonEdges);
   if(mLatEdges.size() <= 1 && mLonEdges.size() <= 1) {
      Global::logger->write("One of 'latEdges' and 'lonEdges' must be provided, and must have length 2 or more", Logger::error);
   }
   std::sort(mLatEdges.begin(), mLatEdges.end());
   std::sort(mLonEdges.begin(), mLonEdges.end());

   iOptions.check();
}

int PoolerLatLon::findCore(const Location& iLocation) const {
   float lat = iLocation.getLat();
   float lon = iLocation.getLon();
   if(!Global::isValid(lat) || !Global::isValid(lon))
      return Global::MV;
   else {
      int latI = 0;
      int lonI = 0;
      int latSize = 1;
      if(mLatEdges.size() > 1) {
         if(lat < mLatEdges[0])
            return Global::MV;
         for(int i = 1; i < mLatEdges.size(); i++) {
            if(lat < mLatEdges[i]) {
               latI = i-1;
               break;
            }
         }
         latSize = mLatEdges.size()-1;
      }
      if(mLonEdges.size() > 1) {
         if(lon < mLonEdges[0])
            return Global::MV;
         for(int i = 1; i < mLonEdges.size(); i++) {
            if(lon < mLonEdges[i]) {
               lonI = i-1;
               break;
            }
         }
      }
      // std::cout << lat << " " << lon << " " << latI << std::endl;
      return latI + lonI * latSize;
   }
   return Global::MV;
}
