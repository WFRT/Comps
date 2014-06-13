#include "Elevation.h"

NeighbourhoodElevation::NeighbourhoodElevation(const Options& iOptions) : Neighbourhood(iOptions),
      mMinElevDiff(0),
      mSearchRadius(Global::MV),
      mNum(Global::MV),
      mAlgorithm(0),
      mDistanceWeight(0),
      mElevationWeight(1),
      mNumBest(1),
      mGradientWeight(0) {

   //! Search for nearest elevation-neighbour within this radius (in m)
   iOptions.getValue("searchRadius", mSearchRadius);
   //! Search for nearest neighbour within this radius (in m)
   iOptions.getValue("num", mNum);
   //! Only use the nearest neighbour if its elevation difference is less than this amount (in m)
   iOptions.getValue("minElevDiff", mMinElevDiff);

   //! What weight should the distance have? (units 1/m)
   iOptions.getValue("distanceWeight", mDistanceWeight);
   //! How weight should the elevation have? (units 1/m)
   iOptions.getValue("elevationWeight", mElevationWeight);
   //! How weight should the gradient have? (units 1m/1m)
   iOptions.getValue("gradientWeight", mGradientWeight);
   //! How many of the best neighbours should be used?
   iOptions.getValue("numBest", mNumBest);

}

std::vector<Location> NeighbourhoodElevation::selectCore(const Input* iInput, const Location& iLocation) const {
   // Get the nearest neighbour, in case we need it
   std::vector<Location> temp;
   iInput->getSurroundingLocations(iLocation, temp);
   std::vector<Location> nearestNeighbour(1,temp[0]);

   float thisElev = iLocation.getElev();
   // Cannot downscale if we do not have elevation information
   if(!Global::isValid(thisElev)) {
      return nearestNeighbour;
   }

   // Check if the nearest neighbour is close enough in elevation
   // float elevNearestNeighbour = nearestNeighbour.getElev();
   // if(!Global::isValid(elevNearestNeighbour) || fabs(elevNearestNeighbour - thisElev) < mMinElevDiff) {
   //    // The true nearest neighbour is close enough in elevation to the desired point.
   //    return nearestNeighbour;
   // }

   // Get a neighbourhood of locations
   std::vector<Location> locations;
   if(Global::isValid(mNum))
      iInput->getSurroundingLocations(iLocation, locations, mNum);
   else if(Global::isValid(mSearchRadius))
      iInput->getSurroundingLocationsByRadius(iLocation, locations, mSearchRadius);
   else
      locations = iInput->getLocations();

   // Check that there are locations within the search radius
   if(locations.size() == 0) {
      return nearestNeighbour;
   }

   // Compute gradient at desired point
   float thisGradX = iLocation.getGradientX();
   float thisGradY = iLocation.getGradientY();

   std::vector<std::pair<int,float> > values; // location index, factor
   for(int i = 0; i < locations.size(); i++) {
      // Compute gradient factor
      float gradientFactor = 0;
      if(mGradientWeight > 0) {
         float gradX       = locations[i].getGradientX();
         float gradY       = locations[i].getGradientY();
         if(Global::isValid(gradX) && Global::isValid(gradY)) {
            float lenGrad     = sqrt(gradX*gradX + gradY*gradY + 1);
            float lenThisGrad = sqrt(thisGradX*thisGradX + thisGradY*thisGradY + 1);
            float cosAngle    = (gradX*thisGradX + gradY*thisGradY + 1) / lenGrad / lenThisGrad;
            gradientFactor    = fabs(1 - cosAngle);
         }
         else {
            gradientFactor = Global::MV;
         }
      }

      // Compute distance factor
      float distanceFactor = 0;
      if(mDistanceWeight > 0) {
         distanceFactor = iLocation.getDistance(locations[i]);
      }

      // Compute elevation factor
      float elevationFactor = 0;
      if(mElevationWeight > 0) {
         float elev = locations[i].getElev();
         if(Global::isValid(elev))
            elevationFactor = fabs(elev - thisElev);
         else
            elevationFactor = Global::MV;
      }

      // Compute total factor
      if(Global::isValid(distanceFactor) && Global::isValid(gradientFactor) && Global::isValid(elevationFactor)) {
         float totalFactor = gradientFactor*mGradientWeight + distanceFactor*mDistanceWeight + elevationFactor * mElevationWeight;
         values.push_back(std::pair<int,float>(i, totalFactor));
      }
   }
   if(values.size() > 0) {
      std::sort(values.begin(), values.end(), Global::sort_pair_second<int, float>());
      int N = std::min((int) values.size(), mNumBest);
      std::vector<Location> returnLocations(N);
      for(int i = 0; i < N; i++) {
         int index = values[i].first;
         returnLocations[i] = locations[index];
      }
      return returnLocations;
   }
   else {
      return nearestNeighbour;
   }
}
