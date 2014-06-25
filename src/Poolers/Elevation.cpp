#include "Elevation.h"
#include "../Global.h"
#include "../Options.h"
#include "../Location.h"
#include "../Inputs/Input.h"
#include "../Data.h"
#include "../Location.h"

PoolerElevation::PoolerElevation(const Options& iOptions, const Data& iData) : Pooler(iOptions, iData) {
   //! How wide a band should be used for elevations? [0, bandwidth, 2*bandwidth, ...]
   iOptions.getRequiredValue("bandwidth", mBandwidth);
}

int PoolerElevation::findCore(const Location& iLocation) const {
   float elev = iLocation.getElev();
   if(!Global::isValid(elev))
      return Global::MV;
   else
      return (int) elev / mBandwidth;

}
