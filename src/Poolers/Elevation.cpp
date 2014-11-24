#include "Elevation.h"
#include "../Global.h"
#include "../Options.h"
#include "../Location.h"
#include "../Inputs/Input.h"
#include "../Data.h"
#include "../Location.h"

PoolerElevation::PoolerElevation(const Options& iOptions, const Data& iData) : Pooler(iOptions, iData) {
   //! What bins should elevation be split into?
   iOptions.getRequiredValues("edges", mEdges);
   std::sort(mEdges.begin(), mEdges.end());

   iOptions.check();
}

int PoolerElevation::findCore(const Location& iLocation) const {
   float elev = iLocation.getElev();
   if(!Global::isValid(elev))
      return Global::MV;
   else {
      if(elev < mEdges[0])
         return Global::MV;
      for(int i = 1; i < mEdges.size(); i++) {
         if(elev < mEdges[i]) {
            return i-1;
         }
      }
   }
   return Global::MV;
}
