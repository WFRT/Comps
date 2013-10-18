#include "ParameterIo.h"
#include "SchemesHeader.inc"
#include "../Data.h"
#include "../Configurations/Configuration.h"

const std::string ParameterIo::mBaseOutputDirectory = "./results/";
ParameterIo::ParameterIo(const Options& iOptions, const Data& iData) : Component(iOptions, iData) {

   // Set finders
   std::string finderTag;
   iOptions.getRequiredValue("finder", finderTag);
   Options opt;
   Scheme::getOptions(finderTag, opt);

   mRunDirectory = iData.getRunName();

   // Locations & Offsets
   iData.getOutputOffsets(mOffsets);
   for(int i = 0; i < (int) mOffsets.size(); i++) {
      mOffsetMap[mOffsets[i]] = i;
   }

   iData.getOutputLocations(mOutLocations);
   //iData.getObsInput()->getLocations(mParLocations);
   // TODO: ???
   Location parLocation("single", 0);
   parLocation.setValues(49,-123,0);
   mParLocations.push_back(parLocation);
   Location parLocation2("single", 0);
   parLocation.setValues(52,-122,0);
   mParLocations.push_back(parLocation2);
   createOutParMap();

   // Components
   mComponents.push_back(Component::TypeSelector);
   mComponents.push_back(Component::TypeDownscaler);
   mComponents.push_back(Component::TypeCorrector);
   mComponents.push_back(Component::TypeUncertainty);
   mComponents.push_back(Component::TypeCalibrator);
   mComponents.push_back(Component::TypeAverager);
   mComponents.push_back(Component::TypeSmoother);
   for(int i = 0; i < (int) mComponents.size(); i++) {
      mComponentMap[mComponents[i]] = i;
   }
   mCache.setName("Parameters");
}
ParameterIo::~ParameterIo() {
}

#include "Schemes.inc"

bool ParameterIo::read(Component::Type iType,
      int iDate,
      int iInit,
      float iOffset,
      int iRegion,
      const std::string iVariable,
      const Configuration& iConfiguration,
      int iIndex,
      Parameters& iParameters) const {

   Key::Par key(iType, iDate, iInit, iOffset, iRegion, iVariable, iConfiguration.getName(), iIndex);
   if(mCache.isCached(key)) {
      //std::cout << "   parameter cache HIT " << Component::getName(iType) << "\n";
      iParameters = mCache.get(key);
      return true;
   }
   else {
      //std::cout << "   parameter cache MISS " << Component::getName(iType) << "\n";
      Parameters parameters;
      if(readCore(key, parameters)) {
         iParameters = parameters;
         return true;
      }
      else {
         return false;
      }
   }
}
void ParameterIo::add(Component::Type iType,
      int iDate,
      int iInit,
      float iOffset,
      int iRegion,
      const std::string iVariable,
      const Configuration& iConfiguration,
      int iIndex,
      Parameters iParameters) {
   Key::Par key(iType, iDate, iInit, iOffset, iRegion, iVariable, iConfiguration.getName(), iIndex);
   mParametersWrite[key] = iParameters;
   mCache.add(key, iParameters);
}

void ParameterIo::write() {
   std::map<Key::Par, Parameters>::const_iterator it;
   for(it = mParametersWrite.begin(); it != mParametersWrite.end(); it++) {
      Key::Par key = it->first;
      Parameters par = it->second;
      // Add to cache
      if(key.mType == Component::TypeUncertainty)
         std::cout << par.size() << std::endl;
      mCache.add(key, par);
   }
   writeCore();
   // Clear parameters so they are not rewritten later on
   mParametersWrite.clear();
}
void ParameterIo::createOutParMap() {
   // Find what parameter location each output location belongs to
   for(int i = 0; i < (int) mOutLocations.size(); i++) {
      Location currLocation = mOutLocations[i];
      int closestId = Global::MV;
      double closestDist = Global::INF;
      for(int k = 0; k < (int) mParLocations.size(); k++) {
         double currDist = currLocation.getDistance(mParLocations[k]);
         if(currDist < closestDist) {
            closestDist = currDist;
            closestId = k;
         }
      }
      assert(closestId != Global::MV);
      mOutParMap[mOutLocations[i].getId()] = closestId;
   }
}
