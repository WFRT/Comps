#include "Neighbourhood.h"
#include "../Field.h"
#include "../Data.h"
#include "../Location.h"
DownscalerNeighbourhood::DownscalerNeighbourhood(const Options& iOptions) : Downscaler(iOptions), mWeightOrder(0) {
   std::string neighbourhoodTag;
   //! Which neighbourhood scheme should be used to define the neighbourhood? Defaults to 1 nearest
   //! neighbour.
   if(iOptions.getValue("neighbourhood", neighbourhoodTag)) {
      mNeighbourhood = Neighbourhood::getScheme(neighbourhoodTag);
   }
   else {
      // Default to nearest neighbour
      mNeighbourhood = Neighbourhood::getScheme(Options("class=NeighbourhoodNearest num=1"));
   }

   //! Should gridpoints be weighted by distance? If so, what (inverse) power should be used? If 0 then no weighting performed.
   iOptions.getValue("weightOrder", mWeightOrder);
   if(mWeightOrder < 0) {
      Global::logger->write("DownscalerNeighbourhood: Inverse distance order used in weighting must be positive", Logger::error);
   }
}

float DownscalerNeighbourhood::downscale(const Input* iInput,
      int iDate, int iInit, float iOffset,
      const Location& iLocation,
      int iMemberId,
      const std::string& iVariable) const {

   std::vector<Location> useLocations;
   useLocations = mNeighbourhood->select(iInput, iLocation);

   float total = 0;
   float totalFactor = 0;
   for(int i = 0; i < useLocations.size(); i++) {
      float value = iInput->getValue(iDate, iInit, iOffset, useLocations[i].getId(), iMemberId, iVariable);
      float factor = 1;
      if(mWeightOrder > 0) {
         float dist = iLocation.getDistance(useLocations[i]);
         if(dist == 0) {
            // Location matches exactly, so use this value
            return value;
         }
         factor = pow(dist, -mWeightOrder);
      }
      if(Global::isValid(value)) {
         total += factor * value;
         totalFactor += factor;
      }
   }
   if(totalFactor > 0)
      return total / totalFactor;
   return Global::MV;
}
