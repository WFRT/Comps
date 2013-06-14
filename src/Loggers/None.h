#ifndef LOGGER_NONE_H
#define LOGGER_NONE_H
#include "Logger.h"

//! Records no messages. Useful when doing unit testing
class LoggerNone : public Logger {
   public:
      LoggerNone(Logger::Level rMaxLevel=Logger::debug) : Logger(rMaxLevel) {};
      void write(const std::string& rMessage, Logger::Level rLevel=Logger::critical, Logger::Type = Logger::typeMessage) { if(rLevel==Logger::error) {std::cout << rMessage << std::endl; assert(0);}};
      Logger::Level getMaxLevel() const { return Logger::error;} ;
      void setMaxLevel(Logger::Level rMaxLevel) {};
};
#endif

