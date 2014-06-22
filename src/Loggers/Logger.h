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
      Logger(Logger::Level iMaxLevel=Logger::debug);
      virtual ~Logger();

      virtual void write(const std::string& iMessage, Logger::Level iLevel=Logger::critical, Logger::Type = Logger::typeMessage) = 0;
      virtual void writeProgress(const std::string& iMessage, Logger::Level iLevel=Logger::critical) {};
      void setCurrentLocation(const Location* iLocation, int iCurrentIndex, int iMaxIndex);
      void setCurrentDate(int iDate, int iCurrentIndex, int iMaxIndex);
      //! Set the current configuration to this index
      void setCurrentVarConf(std::string iVariable, const Configuration* iConfiguration);
      void registerCache(const std::string* iName, const float* iSize, const float* iMaxSize, const int* iCacheMisses);
      void unRegisterCache(const std::string* iName, const float* iSize, const float* iMaxSize, const int* iCacheMisses);

      Logger::Level getMaxLevel() const;
      void setMaxLevel(Logger::Level iMaxLevel);
      void setVarConfs(std::map<std::string, std::vector<Configuration*> > iVarConfs);
   protected:
      virtual void setCurrentLocationCore() {};
      virtual void setCurrentDateCore() {};
      virtual void setCurrentVarConfCore() {};
      virtual void setVarConfsCore() {};
      std::map<Level,int> mColourMap;
      Logger::Level mMaxLevel;
      // Variable-configurations
      std::map<std::string, std::vector<Configuration*> > mVarConfs;
      const Configuration* mCurrentConfiguration;
      std::string mCurrentVariable;
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

