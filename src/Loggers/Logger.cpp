#include "Logger.h"
#include "SchemesHeader.inc"

Logger::Logger(Logger::Level iMaxLevel) : mMaxLevel(iMaxLevel) {
   init();
}
Logger::~Logger() {
   //mStream.close();
}

void Logger::init() {
   // Define colours
   mColourMap[Logger::error] = 31;  // Red
   mColourMap[Logger::status] = 32; // Green
   mColourMap[Logger::critical] = 35;  // Magenta
   mColourMap[Logger::message] = 32; // Green
   mColourMap[Logger::warning] = 33; // Yellow

   mCurrLocation = NULL;
   mCurrConfiguration = 0;
}

Logger::Level Logger::getMaxLevel() const {
   return mMaxLevel;
}
void Logger::setMaxLevel(Logger::Level iMaxLevel) {
   mMaxLevel = iMaxLevel;
}

void Logger::setConfigurations(std::vector<Configuration*> iConfigurations) {
   mConfigurations = iConfigurations;
}

void Logger::setLocationInfo(const Location* iLocation, int iCurrentIndex, int iMaxIndex) {
   mCurrLocation = iLocation;
   mNumLocations = iMaxIndex;
   mCurrLocationIndex = iCurrentIndex;
   setLocationCore();
}

void Logger::setDateInfo(int iDate, int iCurrentIndex, int iMaxIndex) {
   mCurrDate = iDate;
   mNumDates = iMaxIndex;
   mCurrDateIndex = iCurrentIndex;
   setDateCore();
}
 
void Logger::registerCache(const std::string* iName, const float* iCurrSize, const float* iMaxSize, const int* iCacheMisses) {
   mCaches[iName] = CacheInfo(iCurrSize, iMaxSize, iCacheMisses);
   mCacheNames.push_back(iName);
   mCacheSizes.push_back(iCurrSize);
   mCacheMaxSizes.push_back(iMaxSize);
   mCacheMisses.push_back(iCacheMisses);
}

void Logger::unRegisterCache(const std::string* iName, const float* iCurrSize, const float* iMaxSize, const int* iCacheMisses) {
   std::map<const std::string*, CacheInfo>::iterator it = mCaches.find(iName);
   if(it != mCaches.end()) {
      mCaches.erase(it);
   }
}

void Logger::setConfigurationInfo(int iIndex) {
   mCurrConfiguration = iIndex;
   setConfigurationsCore();
}
