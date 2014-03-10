#include "Elevation.h"
#include "../Field.h"
#include "../Data.h"
#include "../Location.h"
#include "../Variables/Variable.h"
DownscalerElevation::DownscalerElevation(const Options& iOptions) : Downscaler(iOptions),
      mNumPoints(4),
      mLapseRate(9.8),
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
         for(int i = 0; i < locations.size(); i++) {
            float x = locations[i].getElev();
            float y = iInput->getValue(iDate, iInit, iOffset, locations[i].getId(), iMemberId, iVariable);
            if(Global::isValid(x) && Global::isValid(y)) {
               meanXY += x*y;
               meanX  += x;
               meanY  += y;
               meanXX += x*x;
               counter++;
            }
         }
         if(counter == 0) {
            // No valid elevations or forecasts
            value = Global::MV;
         }
         else if(meanXX == meanX*meanX) {
            // All points are at the same elevation. Default to using a standard lapse rate
            value = followDryAdiabat(meanY, meanX, desiredElevation);
         }
         else {
            meanXY /= counter;
            meanX  /= counter;
            meanY  /= counter;
            meanXX /= counter;
            float b = (meanXY - meanX*meanY)/(meanXX - meanX*meanX);
            // Check against unreasonable lapse rates
            if(b < -10.0/1000)
               b = -10.0/1000; // extreme unstable lapse rates
            if(b > 10.0/1000)
               b = 10.0/1000; // extreme stable lapse rates
            float a = meanY - b * meanX;
            value   = a + b * desiredElevation;
            if(value < -50) {
               std::cout << "# " << b*1000 << " " << value << std::endl;
            }
         }
      }
      // Bring each neighbouring point up/down to desired location and take the average
      else {
         float total   = 0;
         int   counter = 0;
         for(int i = 0; i < locations.size(); i++) {
            float currElevation = locations[i].getElev();
            float currValue = iInput->getValue(iDate, iInit, iOffset, locations[i].getId(), iMemberId, iVariable);
            float adjustedValue = followDryAdiabat(currValue, currElevation, desiredElevation);
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
float DownscalerElevation::followDryAdiabat(float iTemperatureStart, float iZStart, float iZEnd)  const {
   float temperatureEnd = Global::MV;
   if(Global::isValid(iZStart) && Global::isValid(iZEnd) && Global::isValid(iTemperatureStart)) {
      temperatureEnd = iTemperatureStart - mLapseRate/1000 * (iZEnd - iZStart);
   }
   return(temperatureEnd);
}
