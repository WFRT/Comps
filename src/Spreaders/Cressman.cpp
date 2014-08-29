#include "Cressman.h"
#include "../Poolers/Pooler.h"

SpreaderCressman::SpreaderCressman(const Options& iOptions, const Data& iData) : Spreader(iOptions, iData),
      mMaxDistance(Global::MV) {
   iOptions.getValue("maxDistance", mMaxDistance);
   iOptions.getRequiredValue("efoldDistance", mEfoldDistance);
   iOptions.check();
}

bool SpreaderCressman::estimate(const ParameterIo& iParameterIo,
         const Pooler& iPooler,
         Processor::Type iType,
         int iDate,
         int iInit,
         float iOffsetCode,
         const Location& iLocation,
         const std::string iVariable,
         int iIndex,
         Parameters& iParameters) const {
   // Get nearest location
   std::vector<Location> locations;
   mData.getObsInput()->getSurroundingLocations(iLocation, locations, 1);
   if(locations.size() != 1)
      return false;
   Location nearestNeighbour = locations[0];

   float dist = iLocation.getDistance(nearestNeighbour);

   // Get parameters at nearest location
   int poolId = iPooler.find(nearestNeighbour);
   bool status = iParameterIo.read(iType, iDate, iInit, iOffsetCode, poolId, iVariable, iIndex, iParameters);
   // TODO: Add methods to processors, so that we can figure out how to adjust the parameters
   if(!status || iType != Processor::TypeCorrector)
      return false;

   for(int k = 0; k < iParameters.size(); k++) {
      if(Global::isValid(iParameters[k])) {
         float value = iParameters[k] * exp(-dist/mEfoldDistance);
         iParameters[k] = value;
      }
   }
   return true;
}
