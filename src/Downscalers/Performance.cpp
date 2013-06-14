#include "Performance.h"
#include "../DetMetrics/DetMetric.h"
#include "../Scheme.h"
#include "../Location.h"
#include "../Data.h"
#include "../Inputs/Input.h"
#include "../Parameters.h"
#include "../Slice.h"
#include "../Obs.h"

DownscalerPerformance::DownscalerPerformance(const Options& iOptions, const Data& iData) : Downscaler(iOptions, iData) {
   //Component::underDevelopment();
   iOptions.getRequiredValue("numPoints", mNumPoints);
   if(mNumPoints <= 0) {
      Global::logger->write("DownscalerPerformance: Number of interpolation points must be greater than 0", Logger::error);
   }
   std::string metricTag;
   iOptions.getRequiredValue("detMetric", metricTag);
   Options optMetric("");
   Scheme::getOptions(metricTag, optMetric);
   mMetric = DetMetric::getScheme(optMetric, iData);
}
DownscalerPerformance::~DownscalerPerformance() {
   delete mMetric;
}
float DownscalerPerformance::downscale(const Slice& iSlice,
      const std::string& iVariable,
      const Location& iLocation,
      const Parameters& iParameters) const {
   Parameters parDownscaler;
   iParameters.getSubset(0, parDownscaler);

   std::vector<Location> locations;
   Input* input = mData.getInput(iSlice.getMember().getDataset());
   input->getSurroundingLocations(iLocation, locations, mNumPoints);

   // Find best preforming location
   int index = 0;
   float bestPerformance = parDownscaler[0];
   float factor = 1;
   // Swap the sign of the performance if metric is positively oriented
   if(mMetric->isPositiveOriented()) {
      factor = -1;
   }
   // Find minimum
   //std::cout << "Performance " << std::endl;
   for(int i = 0; i < (int) locations.size(); i++) {
      float currPar = parDownscaler[i];
      if(currPar != Global::MV && currPar < bestPerformance) {
         index = i;
         bestPerformance = factor*currPar;
      }
      //std::cout << " " << currPar << " " << locations[i].getLat() << " " << locations[i].getLon();
   }
   //std::cout << std::endl;
   //std::cout << "Best index: " << index << " " << bestPerformance << std::endl;

   return mData.getValue(iSlice.getDate(), iSlice.getInit(), iSlice.getOffset(), locations[index], iSlice.getMember(), iVariable);
}
void DownscalerPerformance::updateParameters(const Slice& iSlice,
      const std::string& iVariable,
      const Location& iLocation,
      const Obs& iObs,
      Parameters& iParameters) const {

   float obsValue      = iObs.getValue();
   if(obsValue != Global::MV) {
      std::vector<Location> locations;
      Input* input = mData.getInput(iSlice.getMember().getDataset());
      input->getSurroundingLocations(iLocation, locations, mNumPoints);

      Parameters parDownscaler;
      iParameters.getSubset(0, parDownscaler);
      assert((int) parDownscaler.size() == mNumPoints);

      Parameters parMetric;
      iParameters.getSubset(1, parMetric);
      for(int i = 0; i < (int) locations.size(); i++) {
         float originalValue = downscale(iSlice, iVariable, locations[i], iParameters);
         float newScore = mMetric->compute(obsValue, originalValue, parMetric, mData, iVariable);
         parDownscaler[i] = Component::combine(parDownscaler[i], newScore);
         //std::cout << "Updating: " << i << " " << parDownscaler[i] << " " << newScore << std::endl;
      }
      iParameters.setSubset(0, parDownscaler);

      // Update metric scores
      // TODO
      //mMetric->updateParameters(parMetrics);
   }
}

void DownscalerPerformance::getDefaultParameters(Parameters& iParameters) const {
   std::vector<int> records;
   records.push_back(0);
   records.push_back(mNumPoints);
   iParameters.setRecords(records);

   // Downscaler parameters
   Parameters parDownscaler;
   for(int i = 0; i < mNumPoints; i++) {
      parDownscaler[i] = 0;
   }
   iParameters.setSubset(0, parDownscaler);

   // Metric parameters
   Parameters parMetric;
   mMetric->getDefaultParameters(parMetric);
   iParameters.setSubset(1, parMetric);
}
