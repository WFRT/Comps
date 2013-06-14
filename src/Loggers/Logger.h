#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <assert.h>
#include <limits>
#include <math.h>

/** Class for writing runtime status and error messagess
    Only log messages that are less than mMaxLevel
*/
class Configuration;
class Location;

class Logger {
   public:
      enum Level {error = 0, status = 5, critical = 10, warning = 50, message = 100, debug = 1000};
      enum Type  {typeStatus = 0, typeMessage = 10, typeProgress = 20};
      Logger(Logger::Level rMaxLevel=Logger::debug);
      ~Logger();

      virtual void write(const std::string& rMessage, Logger::Level rLevel=Logger::critical, Logger::Type = Logger::typeMessage) = 0;
      virtual void writeProgress(const std::string& rMessage, Logger::Level rLevel=Logger::critical) {};
      void setLocationInfo(const Location* iLocation, int iCurrentIndex, int iMaxIndex);
      void setDateInfo(int iDate, int iCurrentIndex, int iMaxIndex);
      void setConfigurationInfo(int iIndex);
      void registerCache(const std::string* iName, const float* iSize, const float* iMaxSize, const int* iCacheMisses);
      void unRegisterCache(const std::string* iName, const float* iSize, const float* iMaxSize, const int* iCacheMisses);

      Logger::Level getMaxLevel() const;
      void setMaxLevel(Logger::Level rMaxLevel);
      void setConfigurations(std::vector<Configuration*> iConfiguration);
   protected:
      virtual void setLocationCore() {};
      virtual void setDateCore() {};
      virtual void setConfigurationsCore() {};
      std::map<Level,int> mColourMap;
      Logger::Level mMaxLevel;
      std::vector<Configuration*> mConfigurations;
      int mCurrConfiguration;
      // Location
      const Location* mCurrLocation;
      int mNumLocations;
      int mCurrLocationIndex;
      // Date
      int mCurrDate;
      int mNumDates;
      int mCurrDateIndex;
      class CacheInfo {
         public:
            CacheInfo() : mSize(NULL), mMaxSize(NULL) {};
            CacheInfo(const float* iSize, const float* iMaxSize, const int* iMisses) : mSize(iSize), mMaxSize(iMaxSize), mMisses(iMisses) {};
            const float* mSize;
            const float* mMaxSize;
            const int* mMisses;
      };
      std::vector<const std::string*> mCacheNames;
      std::map<const std::string*, CacheInfo> mCaches;
      std::vector<const float*> mCacheSizes;
      std::vector<const int*> mCacheMisses;
      std::vector<const float*> mCacheMaxSizes;
      std::vector<float> mCacheTrimSizes;

   private:
      void init();
};
#endif

