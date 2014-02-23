#include "Interpolate.h"
#include "../Poolers/Pooler.h"

SpreaderInterpolate::SpreaderInterpolate(const Options& iOptions, const Data& iData) : Spreader(iOptions, iData) {
   mNum = 4;
}

bool SpreaderInterpolate::estimate(const ParameterIo& iParameterIo,
         const Pooler& iPooler,
         Component::Type iType,
         int iDate,
         int iInit,
         float iOffsetCode,
         const Location& iLocation,
         const std::string iVariable,
         int iIndex,
         Parameters& iParameters) const {
   std::vector<Location> locations;
   mData.getObsInput()->getSurroundingLocations(iLocation, locations, 4);

   assert(locations.size() > 0);

   std::vector<float> distances(locations.size(), Global::MV);
   for(int i = 0; i < locations.size(); i++) {
      Location location = locations[i];
      float dist = iLocation.getDistance(location);
      if(dist == 0) {
         locations.clear();
         locations.push_back(location);
         distances.clear();
         distances.push_back(1);
         break;
      }
      distances[i] = dist;
   }

   assert(locations.size() == distances.size());
   std::vector<float> values;
   std::vector<float> totalWeight;
   // Average the parameter values
   for(int i = 0; i < locations.size(); i++) {
      float dist = distances[i];
      assert(dist > 0);
      float weight = 1.0 / dist;
      int poolId = iPooler.find(locations[i]);
      Parameters parameters;
      bool status = iParameterIo.read(iType, iDate, iInit, iOffsetCode, poolId, iVariable, iIndex, parameters);
      if(status && values.size() == 0) {
         values.resize(parameters.size(), 0);
         totalWeight.resize(parameters.size(), 0);
      }
      if(status && parameters.size() == values.size()) {
         for(int k = 0; k < parameters.size(); k++) {
            if(Global::isValid(parameters[k])) {
               values[k] += parameters[k]*weight;
               totalWeight[k] += weight;
            }
         }
      }
   }
   if(values.size() == 0) {
      return false;
   }
   for(int k = 0; k < values.size(); k++) {
      if(totalWeight[k] > 0) {
         values[k] /= totalWeight[k];
      }
      else {
         values[k] = Global::MV;
      }
   }
   iParameters.setAllParameters(values);
   return true;
}
