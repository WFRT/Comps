#include "Configuration.h"
#include "Default.h"
#include "../Uncertainties/Uncertainty.h"
#include "../Selectors/Selector.h"
#include "../Downscalers/Downscaler.h"
#include "../Correctors/Corrector.h"
#include "../Continuous/Continuous.h"
#include "../Discretes/Discrete.h"
#include "../Averagers/Averager.h"
#include "../Calibrators/Calibrator.h"
#include "../Estimators/Estimator.h"
#include "../Estimators/MaximumLikelihood.h"
#include "../Smoothers/Smoother.h"
#include "../ParameterIos/ParameterIo.h"
#include "../Poolers/Pooler.h"
#include "../Poolers/Locations.h"
#include "../Spreaders/Spreader.h"
#include "../Spreaders/Pooler.h"

Configuration::Configuration(const Options& iOptions, const Data& iData) :
      Component(iOptions),
      mData(iData),
      mNumDaysParameterSearch(1) {

   iOptions.getRequiredValue("tag", mName);

   //! How many days back should parameters be searched for if yesterday's are missing? A value of 1
   //! means use yesterday's only.
   iOptions.getValue("numDaysParameterSearch", mNumDaysParameterSearch);
   if(mNumDaysParameterSearch == 0 || mNumDaysParameterSearch > 1000) {
      std::stringstream ss;
      ss << "Run option 'numDaysParameterSearch' in " << mName << " is " << mNumDaysParameterSearch << ".";
      if(mNumDaysParameterSearch == 0)
         ss << " No parameters will ever be loaded.";
      else
         ss << " This is a long period, and might slow the program down a lot.";
      Global::logger->write(ss.str(), Logger::warning);
   }

   std::string parameterIoTag;
   //! Which IO scheme should be used for storing/retrieving parameters? Default is to store them in
   //! memory only.
   if(iOptions.getValue("parameterIo", parameterIoTag)) {
      mParameters = ParameterIo::getScheme(parameterIoTag, getName(), mData);
   }
   else {
      Options parameterIoOpt = Options("tag=test class=ParameterIoMemory");
      mParameters = ParameterIo::getScheme(parameterIoOpt, getName(), mData);
   }

   std::string poolerTag;
   //! Which scheme for pooling observations for estimating parameters should be used?
   //! Default to using one set of parameters at each observation location.
   if(iOptions.getValue("pooler", poolerTag)) {
      mPooler = Pooler::getScheme(poolerTag, mData);
   }
   else {
      mPooler = new PoolerLocations(Options(), mData);
   }

   std::string spreaderTag;
   //! Which scheme should be used to spread parameters spatially and temporaly?
   //! Default to using the scheme that pools observations together.
   if(iOptions.getValue("spreader", spreaderTag)) {
      mSpreader = Spreader::getScheme(spreaderTag, mData);
   }
   else {
      mSpreader = new SpreaderPooler(Options(), mData);
   }
}

Configuration::~Configuration() {
   for(int i = 0; i < (int) mProcessors.size(); i++) {
      delete mProcessors[i];
   }
   delete mPooler;
   delete mSpreader;
}


// TODO: For now always use the default configuration, so that we don't have to add
// class=ConfigurationDefault on every line in the configurations.nl file
Configuration* Configuration::getScheme(const Options& iOptions, const Data& iData) {
   Options opt = iOptions;
   opt.addOption("class", "ConfigurationDefault");
   return new ConfigurationDefault(opt, iData);
}
Configuration* Configuration::getScheme(const std::string& iTag, const Data& iData) {
   Options opt = getOptions(iTag);
   return getScheme(opt, iData);
}

void Configuration::init() {
   std::string message;
   if(!isValid(message)) {
      std::stringstream ss;
      ss << "Configuration " << mName << " is not valid. " << message << ".";
      Global::logger->write(ss.str(), Logger::error);
   }
}

std::vector<const Processor*> Configuration::getProcessors(Processor::Type iType) const {
   std::vector<const Processor*> processors;
   for(int i = 0; i < mProcessors.size(); i++) {
      if(mProcessors[i]->getType() == iType) {
         processors.push_back(mProcessors[i]);
      }
   }
   return processors;
}

Options Configuration::getOptions(const std::string& iTag) {
   Options opt;
   int dotPosition = -1;
   for(int i = 0; i < iTag.size(); i++) {
      if(iTag[i] == '.')
         dotPosition = i;
   }
   std::string tag;
   std::string folder;
   if(dotPosition == -1) {
      folder = "default";
      tag = iTag;
   }
   else {
      assert(dotPosition != 0);
      folder = iTag.substr(0,dotPosition);
      tag = iTag.substr(dotPosition+1);//, iTag.size()-2);
   }

   Namelist nl("configurations", folder);

   if(!nl.getOptions(tag, opt)) {
      std::stringstream ss;
      ss << "Configuration " << iTag << " is undefined";
      Global::logger->write(ss.str(), Logger::error);
   }
   return opt;
}

void Configuration::getParameters(Processor::Type iType,
      int iDate,
      int iInit,
      float iOffsetCode,
      const Location& iLocation,
      const std::string iVariable,
      int iIndex,
      Parameters& iParameters) const {
   // int poolId = mPooler->find(iLocation);
   // getParameters(iType, iDate, iInit, iOffsetCode, poolId, iVariable, iIndex, iParameters);

   int counter = 1;
   bool found = false;

   // Search previous dates for parameters
   while(counter <= mNumDaysParameterSearch) {
      int dateParGet = Global::getDate(iDate, -24*counter);
      //std::cout << "Searching parameters for date " << dateParGet << std::endl;
      // found = mParameters->read(iType, dateParGet, iInit, iOffsetCode, iPoolId, iVariable, iIndex, iParameters);
      found = mSpreader->estimate(*mParameters, *mPooler, iType, dateParGet, iInit, iOffsetCode, iLocation, iVariable, iIndex, iParameters);
      if(found) {
         break;
      }
      else {
         std::stringstream ss;
         ss << "No " << Processor::getProcessorName(iType) << " parameters found for " << dateParGet;
         Global::logger->write(ss.str(), Logger::message);
      }
      counter++;
   }
   // Use default parameters if none are found
   if(!found)  {
      std::vector<const Processor*> components = getProcessors(iType);
      assert(iIndex < components.size());
      components[iIndex]->getDefaultParameters(iParameters);
      if(iParameters.size() > 0) {
         std::stringstream ss;
         ss << "Default (non-trivial) parameters used for: " << Processor::getProcessorName(iType);
         Global::logger->write(ss.str(), Logger::message);
      }
   }


}

void Configuration::getParameters(Processor::Type iType,
      int iDate,
      int iInit,
      float iOffsetCode,
      int iPoolId,
      const std::string iVariable,
      int iIndex,
      Parameters& iParameters) const {
   int counter = 1;
   bool found = false;

   // Search previous dates for parameters
   while(counter <= mNumDaysParameterSearch) {
      int dateParGet = Global::getDate(iDate, -24*counter);
      //std::cout << "Searching parameters for date " << dateParGet << std::endl;
      // TODO: Why does parameterIo need to take a configuration?
      found = mParameters->read(iType, dateParGet, iInit, iOffsetCode, iPoolId, iVariable, iIndex, iParameters);
      if(found) {
         break;
      }
      else {
         std::stringstream ss;
         ss << "No " << Processor::getProcessorName(iType) << " parameters found for " << dateParGet;
         Global::logger->write(ss.str(), Logger::message);
      }
      counter++;
   }
   // Use default parameters if none are found
   if(!found)  {
      std::vector<const Processor*> components = getProcessors(iType);
      assert(iIndex < components.size());
      components[iIndex]->getDefaultParameters(iParameters);
      if(iParameters.size() > 0) {
         std::stringstream ss;
         ss << "Default (non-trivial) parameters used for: " << Processor::getProcessorName(iType);
         Global::logger->write(ss.str(), Logger::message);
      }
   }
   //std::cout << "Get Parameters: " << Processor::getProcessorName(iType) <<  " " << iDate << " " << offsetCode << " " << iLocation.getId() << " : " << offset << " " << region << " : " << iParameters.size() << std::endl;
}
void Configuration::setParameters(Processor::Type iType,
      int iDate,
      int iInit,
      float iOffsetCode,
      int iPoolId,
      const std::string iVariable,
      int iIndex,
      const Parameters& iParameters) {

   //std::cout << "Set Parameters: " << iDate << " " << iOffsetCode << " " << iLocation.getId() << " : " << offset << " " << region << " : " << iParameters.size() << std::endl;

   int   dateParPut   = iDate;//Global::getDate(iDate, -24*(day));
   std::stringstream ss;
   ss << "Setting " << Processor::getProcessorName(iType) << " parameters for : " << iDate << "," << iOffsetCode << " " << dateParPut;
   Global::logger->write(ss.str(), Logger::message);

   mParameters->add(iType, dateParPut, iInit, iOffsetCode, iPoolId, iVariable, iIndex, iParameters);
}

void Configuration::addProcessor(const Processor* iProcessor) {
   mProcessors.push_back(iProcessor);
}
void Configuration::addExtraComponent(const Component* iComponent) {
   mExtraComponents.push_back(iComponent);
}

std::vector<const Processor*> Configuration::getAllProcessors() const {
   return mProcessors;
}

std::vector<const Component*> Configuration::getAllComponents() const {
   std::vector<const Component*> components;
   for(int i = 0; i < mProcessors.size(); i++) {
      components.push_back(mProcessors[i]);
   }
   for(int i = 0; i < mExtraComponents.size(); i++) {
      components.push_back(mExtraComponents[i]);
   }
   return components;
}
std::string Configuration::getName() const {
   return mName;
}

