#ifndef LOGGER_DEFAULT_H
#define LOGGER_DEFAULT_H
#include "Logger.h"

/** Class for writing runtime status and error messagess
    Only log messages that are less than mMaxLevel
*/

class LoggerDefault : public Logger {
   public:
      LoggerDefault(Logger::Level iMaxLevel=Logger::debug);
      LoggerDefault(std::ofstream& iStream, Logger::Level iMaxLevel=Logger::debug);
      ~LoggerDefault();
      void write(const std::string& iMessage, Logger::Level iLevel=Logger::critical, Logger::Type = Logger::typeMessage);
      Logger::Level getMaxLevel() const;
      void setMaxLevel(Logger::Level iMaxLevel);
   private:
      std::ostream& mStream;
      std::string getStartColor(Logger::Level iLevel) const;
      std::string getEndColor(Logger::Level iLevel) const;
      void setLocationCore();
      void setDateCore();
      void setConfigurationsCore();
};
#endif

