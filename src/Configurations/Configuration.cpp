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
#include "../Updaters/Updater.h"
#include "../Estimators/Estimator.h"
#include "../Estimators/MaximumLikelihood.h"
#include "../Smoothers/Smoother.h"
#include "../ParameterIos/ParameterIo.h"

Configuration::Configuration(const Options& iOptions, const Data& iData) :
      Component(iOptions, iData),
      mNumDaysParameterSearch(1) {

   iOptions.getRequiredValue("tag", mName);

   // How many days back to search for parameters. 1 is only yesterday
   iOptions.getValue("numDaysParameterSearch", mNumDaysParameterSearch);
   if(mNumDaysParameterSearch == 0 || mNumDaysParameterSearch > 1000) {
      std::stringstream ss;
      std::string tag;
      iOptions.getRequiredValue("tag", tag);
      ss << "Run option 'numDaysParameterSearch' in " << tag << " is " << mNumDaysParameterSearch << ".";
      if(mNumDaysParameterSearch == 0)
         ss << " No parameters will ever be loaded.";
      else
         ss << " This is a long period, and might slow the program down a lot.";
      Global::logger->write(ss.str(), Logger::warning);
   }

   // Instantiate the parameters for this configuration
   Options parameterIoOpt;
   if(Data::getParameterIo() != "") {
      Scheme::getOptions(Data::getParameterIo(), parameterIoOpt);
   }
   else {
      parameterIoOpt = Options("tag=test class=ParameterIoMemory finder=finder");
   }
   mParameters = ParameterIo::getScheme(parameterIoOpt, mData);
}

Configuration::~Configuration() {
   for(int i = 0; i < (int) mComponents.size(); i++) {
      delete mComponents[i];
   }
}


// TODO: For not always use the default configuration, so that we don't have to add
// class=ConfigurationDefault on every line in the configurations.nl file
Configuration* Configuration::getScheme(const Options& iOptions, const Data& iData) {
   Options opt = iOptions;
   opt.addOption("class=ConfigurationDefault");
   return new ConfigurationDefault(opt, iData);
}
Configuration* Configuration::getScheme(const std::string& iTag, const Data& iData) {
   Options opt;
   getOptions(iTag, opt);
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

std::vector<const Component*> Configuration::getComponents(Component::Type iType) const {
   std::vector<const Component*> components;
   for(int i = 0; i < mComponents.size(); i++) {
      if(mComponentTypes[i] == iType) {
         components.push_back(mComponents[i]);
      }
   }
   return components;
}

void Configuration::getOptions(const std::string& iTag, Options& iOptions) {
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
   std::string optString = nl.findLine(tag);
   if(optString == "") {
      std::stringstream ss;
      ss << "Configuration " << iTag << " is undefined";
      Global::logger->write(ss.str(), Logger::error);
   }
   iOptions = Options(optString);
}

void Configuration::getParameters(Component::Type iType,
      int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string iVariable,
      int iIndex,
      Parameters& iParameters) const {
   float parOffset = iOffset;
   int counter = 1;
   bool found = false;
   // Search previous dates for parameters
   while(counter <= mNumDaysParameterSearch) {
      int dateParGet = Global::getDate(iDate, -24*counter);
      //std::cout << "Searching parameters for date " << dateParGet << std::endl;
      // TODO: Why does parameterIo need to take a configuration?
      found = mParameters->read(iType, dateParGet, iInit, iOffset, iLocation, iVariable, *this, iIndex, iParameters);
      if(found) {
         break;
      }
      else {
         std::stringstream ss;
         ss << "No " << Component::getComponentName(iType) << " parameters found for " << dateParGet;
         Global::logger->write(ss.str(), Logger::message);
      }
      counter++;
   }
   // Use default parameters if none are found
   if(!found)  {
      std::vector<const Component*> components = getComponents(iType);
      assert(iIndex < components.size());
      components[iIndex]->getDefaultParameters(iParameters);
      if(iParameters.size() > 0) {
         std::stringstream ss;
         ss << "Default (non-trivial) parameters used for: " << Component::getComponentName(iType);
         Global::logger->write(ss.str(), Logger::message);
      }
   }
}
void Configuration::setParameters(Component::Type iType,
      int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string iVariable,
      int iIndex,
      const Parameters& iParameters) {
   float parOffset = iOffset;

   int   dateParPut   = iDate;//Global::getDate(iDate, -24*(day));
   std::stringstream ss;
   ss << "Setting " << Component::getComponentName(iType) << " parameters for : " << iDate << "," << iOffset << " " << dateParPut;
   Global::logger->write(ss.str(), Logger::message);

   mParameters->add(iType, dateParPut, iInit, iOffset, iLocation, iVariable, *this, iIndex, iParameters);
}


void Configuration::addComponent(const Component* iComponent, Component::Type iType) {
   mComponents.push_back(iComponent);
   mComponentTypes.push_back(iType);
}

void Configuration::getAllComponents(std::vector<const Component*>& iComponents, std::vector<Component::Type>& iTypes) const {
   iComponents = mComponents;
   iTypes = mComponentTypes;
}
std::string Configuration::getName() const {
   return mName;
}
