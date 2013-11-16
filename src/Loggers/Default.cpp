#include "Default.h"
#include "../Location.h"
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include "../Configurations/Configuration.h"

LoggerDefault::LoggerDefault(Logger::Level iMaxLevel) : Logger(iMaxLevel), mStream(std::cout) {
}
LoggerDefault::LoggerDefault(std::ofstream& iStream, Logger::Level iMaxLevel) : Logger(iMaxLevel), mStream(iStream) {
}
LoggerDefault::~LoggerDefault() {
   //mStream.close();
}

void LoggerDefault::write(const std::string& iMessage, Logger::Level iLevel, Logger::Type iType) {
   if(iLevel <= mMaxLevel) {
      time_t timer;
      time(&timer);
      char buffer[80];

      std::string startCol = getStartColor(iLevel);
      std::string endCol   = getEndColor(iLevel);
      strftime(buffer, 80, "%c", gmtime(&timer));
      mStream << startCol << buffer << " (" << iLevel << ") " << iMessage << endCol << std::endl;
      if(iLevel == LoggerDefault::error) {
         mStream << "Stack trace:" << std::endl;
         void *array[10];
         size_t size = backtrace(array, 10);

         // print out all the frames to stderr
         backtrace_symbols_fd(array, size, 2);
         abort();
      }
   }
   for(int i = 0; i < (int) mCacheNames.size(); i++) {
      //std::cout << "Cache size: " << mCacheNames[i] << " = " << *mCacheSizes[i] << std::endl;
   }

}
std::string LoggerDefault::getStartColor(Logger::Level iLevel) const {
   std::map<Level,int>::const_iterator it = mColourMap.find(iLevel);
   if(it == mColourMap.end()) {
      return "";
   }
   else {
      std::stringstream ss;
      ss << "\033[" << it->second << "m";
      return ss.str();
   }
}
std::string LoggerDefault::getEndColor(Logger::Level iLevel) const {
   std::map<Level,int>::const_iterator it = mColourMap.find(iLevel);
   if(it == mColourMap.end()) {
      return "";
   }
   else {
      return "\033[0m";
   }
}

void LoggerDefault::setCurrentLocationCore() {
   std::stringstream ss;
   ss << "Location: " << mCurrLocation->getId() << " (" << mCurrLocationIndex << "/" << mNumLocations << ")";
   Global::logger->write(ss.str(), Logger::message);
}
void LoggerDefault::setCurrentDateCore() {
   std::stringstream ss;
   ss << "Processing date:" << mCurrDate << " (" << mCurrDateIndex << "/" << mNumDates << ")";
   write(ss.str(), Logger::status);
}

void LoggerDefault::setConfigurationsCore() {
   for(int i = 0; i < mConfigurations.size(); i++) {
      mStream << "Configuration: " << mConfigurations[i]->getName() << std::endl;
      std::string options = mConfigurations[i]->toString();
      mStream << options << std::endl;
   }
}
