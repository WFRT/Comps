#include "Surface.h"
#include "../Data.h"
#include "../Location.h"

VariableSurface::VariableSurface() : Variable("Surface") {
   abort();
}

float VariableSurface::computeCore(const Data& iData,
      int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   assert(0);

   float T  = iData.getValue(iDate, iInit, iOffset, iLocation, iMember, "T");
   //int landUse = iLocation.getLandUse();

   return T;
}
