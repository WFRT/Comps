#include "ParameterIo.h"
#include "SchemesHeader.inc"
#include "../Data.h"
#include "../Poolers/Pooler.h"

const std::string ParameterIo::mBaseOutputDirectory = "./results/";
ParameterIo::ParameterIo(const Options& iOptions, std::string iConfiguration, const Data& iData) :
      Component(iOptions),
      mData(iData),
      mConfiguration(iConfiguration) {

   mRunDirectory = iData.getRunName();

   // Components
   mComponents.push_back(Processor::TypeSelector);
   mComponents.push_back(Processor::TypeCorrector);
   mComponents.push_back(Processor::TypeUncertainty);
   mComponents.push_back(Processor::TypeCalibrator);
   mComponents.push_back(Processor::TypeAverager);
   mComponents.push_back(Processor::TypeUpdater);
   mComponents.push_back(Processor::TypeSmoother);
   for(int i = 0; i < (int) mComponents.size(); i++) {
      mComponentMap[mComponents[i]] = i;
   }
   mCache.setName("Parameters");
}
ParameterIo::~ParameterIo() {
}

#include "Schemes.inc"

bool ParameterIo::read(Processor::Type iType,
      int iDate,
      int iInit,
      float iOffset,
      int iPoolId,
      const std::string iVariable,
      int iIndex,
      Parameters& iParameters) const {

   Key::Par key(iType, iDate, iInit, iOffset, iPoolId, iVariable, iIndex);
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
void ParameterIo::add(Processor::Type iType,
      int iDate,
      int iInit,
      float iOffset,
      int iPoolId,
      const std::string iVariable,
      int iIndex,
      Parameters iParameters) {
   Key::Par key(iType, iDate, iInit, iOffset, iPoolId, iVariable, iIndex);
   mParametersWrite[key] = iParameters;
   mCache.add(key, iParameters);
}

void ParameterIo::write() {
   std::map<Key::Par, Parameters>::const_iterator it;
   for(it = mParametersWrite.begin(); it != mParametersWrite.end(); it++) {
      Key::Par key = it->first;
      Parameters par = it->second;
      // Add to cache
      mCache.add(key, par);
   }
   writeCore(mParametersWrite);
   // Clear parameters so they are not rewritten later on
   mParametersWrite.clear();
}
