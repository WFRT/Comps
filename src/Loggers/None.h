#ifndef LOGGER_NONE_H
#define LOGGER_NONE_H
#include "Logger.h"

//! Records no messages. Useful when doing unit testing
class LoggerNone : public Logger {
   public:
      LoggerNone(Logger::Level iMaxLevel=Logger::debug) : Logger(iMaxLevel) {};
      void write(const std::string& iMessage, Logger::Level iLevel=Logger::critical, Logger::Type = Logger::typeMessage) { if(iLevel==Logger::error) {std::cout << iMessage << std::endl; assert(0);}};
      Logger::Level getMaxLevel() const { return Logger::error;} ;
      void setMaxLevel(Logger::Level iMaxLevel) {};
};
#endif

