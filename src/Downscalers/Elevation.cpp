#include "Elevation.h"
#include "../Field.h"
#include "../Data.h"
#include "../Location.h"
#include "../Variables/Variable.h"
DownscalerElevation::DownscalerElevation(const Options& iOptions, const Data& iData) : Downscaler(iOptions, iData),
      mNumPoints(4),
      mLapseRate(9.8) {
   //! How many nearest neighbours should be averaged?
   iOptions.getValue("numPoints", mNumPoints);
   //! Use this lapse rate (degrees/km). Positive means decreasing temperature with height.
   iOptions.getValue("lapseRate", mLapseRate);
}

float DownscalerElevation::downscale(const Field& iField,
      const std::string& iVariable,
      const Location& iLocation,
      const Parameters& iParameters) const {

   std::string sliceDataset = iField.getMember().getDataset();
   Input* input = mData.getInput(sliceDataset); //mData.getInput(iField.getMember().getDataset());

   const Variable* var = Variable::get(iVariable);
   std::vector<Location> locations;
   input->getSurroundingLocations(iLocation, locations, mNumPoints);

   float value = Global::MV;
   float desiredElevation = iLocation.getElev();

   if(var->getBaseVariable() == "T") {
      float total   = 0;
      int   counter = 0;
      // Bring each neighbouring point up/down to desired location
      for(int i = 0; i < mNumPoints; i++) {
         float currElevation = locations[i].getElev();
         float currValue     = mData.getValue(iField.getDate(), iField.getInit(), iField.getOffset(),
                                              locations[i], iField.getMember(), iVariable);
         float adjustedValue = followDryAdiabat(currValue, currElevation, desiredElevation);
         if(Global::isValid(adjustedValue)) {
            total += adjustedValue;
            counter++;
         }
      }
      // Average the points together
      if(counter > 0)
         value = total / counter;
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
