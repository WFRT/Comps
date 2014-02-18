#include "Distance.h"
#include "../Options.h"
#include "../Inputs/Input.h"
#include "../Data.h"
#include "../Field.h"
#include "../Location.h"
#include "../Obs.h"

DownscalerDistance::DownscalerDistance(const Options& iOptions) : Downscaler(iOptions) {
   //! How many neighbours should be used?
   iOptions.getRequiredValue("numPoints", mNumPoints);
   //! What (inverse) order should be applied to the distance when weighing? Use a positive number.
   iOptions.getRequiredValue("order", mOrder);

   if(mNumPoints == 0) {
      Global::logger->write("DownscalerDistance: Number of interpolation points must be greater than 0", Logger::error);
   }
   if(mOrder < 0) {
      Global::logger->write("DownscalerDistance: Inverse distance order used in weighting must be positive", Logger::error);
   }
}
float DownscalerDistance::downscale(const Input* iInput,
      int iDate, int iInit, float iOffset,
      const Location& iLocation,
      int iMemberId,
      const std::string& iVariable) const {

   std::vector<Location> locations;
   iInput->getSurroundingLocations(iLocation, locations, mNumPoints);

   float total = 0;
   float factorAccum = 0;
   for(int i = 0; i < mNumPoints; i++) {
      if(i < locations.size()) {
         float dist = iLocation.getDistance(locations[i]);
         float currValue = iInput->getValue(iDate, iInit, iOffset, locations[i].getId(), iMemberId, iVariable);
         if(!Global::isValid(currValue) || dist == 0) {
            // Location matches exactly, so use this value
            return currValue;
         }
         float factor = pow(dist,-mOrder);
         if(Global::isValid(dist) && Global::isValid(factor)) {
            factorAccum += factor;
            total += factor * currValue;
            assert(!std::isnan(total));
         }
      }
   }
   float value = Global::MV;
   if(factorAccum > 0)
      value = total / factorAccum;
   return value;
}
