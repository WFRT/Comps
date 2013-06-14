#ifndef LOGGER_NCURSES_H
#define LOGGER_NCURSES_H
#include "Logger.h"
#include <ncurses.h>

/** Class for writing runtime status and error messagess
    Only log messages that are less than mMaxLevel
*/

class LoggerNcurses : public Logger {
   public:
      LoggerNcurses(Logger::Level rMaxLevel=Logger::debug);
      ~LoggerNcurses();
      void write(const std::string& rMessage, Logger::Level rLevel=Logger::critical, Logger::Type = Logger::typeMessage);
   protected:
      void drawProgress();
      void drawStatus();
      void setLocationCore();
      void setDateCore();
      void drawBar(WINDOW* iWin, float iFraction, int iMaxWidth, bool doRight=false);
      void setConfigurationsCore();
   private:
      WINDOW* mWinMessage;  // Message window at the bottom
      WINDOW* mWinProgress; // Shows how far through the analysis we are
      WINDOW* mWinStatus;   // Shows cache sizes etc
      WINDOW* mWinWarning;  // Shows all warnings
      WINDOW* mWinConfig;   // Shows all configurations
      std::string mProgressHeader;
      std::string mStatusHeader;
      std::string mConfigHeader;
      std::string mMessageHeader;
      std::vector<std::string> mMessageBuffer;
      int mMaxMessageBufferSize;
      double mTimePerDate;
      static const float mTimeEfold = 3;
      double mLastTime;
      double mStartTime;
};
#endif

