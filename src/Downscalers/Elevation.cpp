#include "Elevation.h"
#include "../Field.h"
#include "../Data.h"
#include "../Location.h"
#include "../Variables/Variable.h"
DownscalerElevation::DownscalerElevation(const Options& iOptions) : Downscaler(iOptions),
      mNumPoints(1),
      mLapseRate(6.5),
      mComputeLapseRate(false) {
   //! How many nearest neighbours should be averaged?
   iOptions.getValue("numPoints", mNumPoints);
   //! Use this lapse rate (degrees/km). Positive means decreasing temperature with height.
   iOptions.getValue("lapseRate", mLapseRate);
   //! Should the lapse rate be computed from neighbouring points?
   iOptions.getValue("computeLapseRate", mComputeLapseRate);
}

float DownscalerElevation::downscale(const Input* iInput,
      int iDate, int iInit, float iOffset,
      const Location& iLocation,
      int iMemberId,
      const std::string& iVariable) const {

   std::vector<Location> locations;
   iInput->getSurroundingLocations(iLocation, locations, mNumPoints);

   const Variable* var = Variable::get(iVariable);

   float value = Global::MV;
   float desiredElevation = iLocation.getElev();

   if(var->getBaseVariable() == "T") {
      // Find temperature by using linear regression on the elevation and temperatures
      // of neighbouring points
      if(mComputeLapseRate) {
         float meanXY  = 0; // elev*T
         float meanX   = 0; // elev
         float meanY   = 0; // T
         float meanXX  = 0; // elev*elev
         int   counter = 0;
         float nearestT    = Global::MV;
         float nearestElev = Global::MV;
         for(int i = 0; i < locations.size(); i++) {
            float x = locations[i].getElev();
            float y = iInput->getValue(iDate, iInit, iOffset, locations[i].getId(), iMemberId, iVariable);
            if(Global::isValid(x) && Global::isValid(y)) {
               meanXY += x*y;
               meanX  += x;
               meanY  += y;
               meanXX += x*x;
               // Store the temperature at the nearest valid neighbour
               if(!Global::isValid(nearestT)) {
                  nearestT = y;
                  nearestElev = x;
               }
               counter++;
            }
         }
         if(counter == 0) {
            // No valid elevations or forecasts
            value = Global::MV;
         }
         else {
            float lapseRate; // In degrees / km
            // If all points are at the same elevation, use the standard lapse rate
            if(fabs(meanXX - meanX*meanX) < 1) {
               lapseRate = mLapseRate;
            }
            else {
               // Estimate lapse rate
               meanXY /= counter;
               meanX  /= counter;
               meanY  /= counter;
               meanXX /= counter;
               lapseRate = -(meanXY - meanX*meanY)/(meanXX - meanX*meanX)*1000;
               // Check against unreasonable lapse rates
               if(lapseRate < -10.0)
                  lapseRate = -10.0; // extreme unstable lapse rates
               if(lapseRate > 10.0)
                  lapseRate = 10.0; // extreme stable lapse rates
            }

            value = moveParcel(nearestT, nearestElev, desiredElevation, lapseRate);
            assert(Global::isValid(value));
         }
      }
      // Bring each neighbouring point up/down to desired location and take the average
      else {
         float total   = 0;
         int   counter = 0;
         for(int i = 0; i < locations.size(); i++) {
            float currElevation = locations[i].getElev();
            float currValue = iInput->getValue(iDate, iInit, iOffset, locations[i].getId(), iMemberId, iVariable);
            float adjustedValue = moveParcel(currValue, currElevation, desiredElevation, mLapseRate);
            if(Global::isValid(adjustedValue)) {
               total += adjustedValue;
               counter++;
            }
         }
         if(counter > 0)
            value = total / counter;
      }
   }
   else {
      std::stringstream ss;
      ss << "Cannot downscale " << iVariable << " using elevation information";;
      Global::logger->write(ss.str(), Logger::warning);
   }
   return value;
}
float DownscalerElevation::moveParcel(float iTemperatureStart, float iZStart, float iZEnd, float iLapseRate)  const {
   float temperatureEnd = Global::MV;
   if(Global::isValid(iZStart) && Global::isValid(iZEnd) && Global::isValid(iTemperatureStart) && Global::isValid(iLapseRate)) {
      temperatureEnd = iTemperatureStart - iLapseRate/1000 * (iZEnd - iZStart);
   }
   return temperatureEnd;
}
