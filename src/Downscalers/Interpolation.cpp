#include "Interpolation.h"
#include "../Options.h"
#include "../Inputs/Input.h"
#include "../Data.h"
#include "../Slice.h"
#include "../Location.h"
#include "../Obs.h"

DownscalerInterpolation::DownscalerInterpolation(const Options& iOptions, const Data& iData) : Downscaler(iOptions, iData) {
   //Component::underDevelopment();
   iOptions.getRequiredValue("numPoints", mNumPoints);
   iOptions.getRequiredValue("order", mOrder);
   if(mNumPoints == 0) {
      Global::logger->write("DownscalerInterpolation: Number of interpolation points must be greater than 0", Logger::error);
   }
   if(mNumPoints <= mOrder) {
      Global::logger->write("DownscalerInterpolation: Number of interpolation points must be greater than the interpolation order", Logger::error);
   }
}
float DownscalerInterpolation::downscale(const Slice& iSlice,
      const std::string& iVariable,
      const Location& iLocation,
      const Parameters& iParameters) const {
   std::vector<Location> locations;
   Input* input = mData.getInput(iSlice.getMember().getDataset());
   input->getSurroundingLocations(iLocation, locations, mNumPoints);

   float value = 0;
   float factorAccum = 0;
   for(int i = 0; i < mNumPoints; i++) {
      if(i < locations.size()) {
         float dist = iLocation.getDistance(locations[i]);
         float factor = pow(dist,mOrder);
         float currValue = mData.getValue(iSlice.getDate(), iSlice.getInit(), iSlice.getOffset(), locations[i], iSlice.getMember(), iVariable);
         if(!Global::isValid(currValue) || dist == 0) {
            // Location matches exactly, so use this value
            return currValue;
         }
         if(Global::isValid(dist) && Global::isValid(factor)) {
            factorAccum += factor;
            value += factor * currValue;
            assert(!std::isnan(value));
         }
      }
   }
   if(factorAccum == 0)
      value = Global::MV;
   else
      value /= factorAccum;
   return value;
}

void DownscalerInterpolation::getDefaultParameters(Parameters& iParameters) const {
   return;
}
