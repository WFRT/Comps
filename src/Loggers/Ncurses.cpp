#include "Ncurses.h"
#include "../Location.h"
#include "../Configurations/Configuration.h"

LoggerNcurses::LoggerNcurses(Logger::Level iMaxLevel) : Logger(iMaxLevel) {
   // Set up windows
   initscr();
   mWinConfig   = newwin(0,30,0,0);
   mWinProgress = newwin(10,0,0,30);
   mWinStatus   = newwin(18,0,10,30);
   mWinMessage  = newwin(0,0,30,30);

   mMaxMessageBufferSize = getmaxy(mWinMessage) - 1;

   // Set header string
   std::stringstream ssProgress;
   std::stringstream ssStatus;
   std::stringstream ssMessage;
   int screenWidth = getmaxx(mWinProgress);
   for(int q = 0; q < screenWidth - 4; q++) {
      if(q != floor(screenWidth/2) - 1) {
         ssProgress << ' ';
         ssStatus   << ' ';
         ssMessage  << ' ';
      }
      else {
         ssProgress << " Status ";
         ssStatus   << " Cache ";
         ssMessage  << " Messages ";
      }
   }
   mConfigHeader   = "         Config              ";
   mProgressHeader = ssProgress.str();
   mStatusHeader   = ssStatus.str();
   mMessageHeader  = ssMessage.str();
   if(has_colors() == FALSE) {
      endwin();
      printf("Your terminal does not support color\n");
      exit(1);
   }
   start_color();
   init_pair(1, COLOR_RED, COLOR_BLACK);
   init_pair(2, COLOR_GREEN, COLOR_BLACK);
   init_pair(3, COLOR_WHITE, COLOR_RED);
   init_pair(4, COLOR_BLACK, COLOR_YELLOW);
   mTimePerDate = 0;
   mLastTime    = Global::clock();
   mStartTime   = mLastTime;

}
LoggerNcurses::~LoggerNcurses() {
   //mStream.close();
}

void LoggerNcurses::write(const std::string& iMessage, Logger::Level iLevel, Logger::Type iType) {
   if(iType == Logger::typeMessage) {
      if(iLevel <= mMaxLevel) {

         mMaxMessageBufferSize = getmaxy(mWinMessage)-1;

         time_t timer;
         time(&timer);
         char buffer[80];

         strftime(buffer, 80, "%c", gmtime(&timer));

         std::stringstream ss;
         ss << buffer << " (" << iLevel << ") " << iMessage;
         if(mMessageBuffer.size() >= mMaxMessageBufferSize) {
            mMessageBuffer.erase(mMessageBuffer.begin());
         }
         mMessageBuffer.push_back(ss.str());

         wmove(mWinMessage, 0,0);
         wclrtoeol(mWinMessage);
         wattron(mWinMessage, COLOR_PAIR(3)); 
         wprintw(mWinMessage, mMessageHeader.c_str());
         wattroff(mWinMessage, COLOR_PAIR(3)); 

         for(int i = 0; i < mMessageBuffer.size(); i++) {
            wmove(mWinMessage, i+1,0);
            wclrtoeol(mWinMessage);
            wprintw(mWinMessage, mMessageBuffer[mMessageBuffer.size()-1-i].c_str());
            //wprintw(mWinMessage, mMessageBuffer[0].c_str());
         }

         wrefresh(mWinMessage);
         if(iLevel == LoggerNcurses::error) {
            abort();
         }
      }
   }
}

void LoggerNcurses::drawProgress() {
   int width = getmaxx(mWinProgress);

   // Header
   wmove(mWinProgress, 0, 0);

   wattron(mWinProgress, COLOR_PAIR(3)); 
   wprintw(mWinProgress, mProgressHeader.c_str());
   wattroff(mWinProgress, COLOR_PAIR(3)); 

   // Date
   wmove(mWinProgress, 1, 0);
   std::stringstream ss;
   ss << "Date           " << mCurrDate << " (" << mCurrDateIndex << "/" << mNumDates << ")";
   wprintw(mWinProgress, ss.str().c_str());

   // Draw progress bar
   wmove(mWinProgress, 1, 40);
   drawBar(mWinProgress, (float) mCurrDateIndex/(float) mNumDates, width-40);

   // Location
   if(mCurrLocation) {
      wmove(mWinProgress, 2, 0);
      wclrtoeol(mWinProgress);

      std::stringstream ss;
      ss << "Location       " << mCurrLocation->getId() << " (" << mCurrLocationIndex << "/" << mNumLocations << ")";
      wprintw(mWinProgress, ss.str().c_str());

      wmove(mWinProgress, 2, 40);
      // Draw progress bar
      drawBar(mWinProgress, (float) mCurrLocationIndex/(float) mNumLocations, width-40);

   }

   wmove(mWinProgress, 4, 0);
   wclrtoeol(mWinProgress);
   std::stringstream ss1;
   ss1 << "Time per date  " << mTimePerDate;
   wprintw(mWinProgress, ss1.str().c_str());

   wmove(mWinProgress, 5, 0);
   wclrtoeol(mWinProgress);
   std::stringstream ss2;
   ss2 << "Current time   " << mLastTime - mStartTime << "s      ";
   wprintw(mWinProgress, ss2.str().c_str());

   wmove(mWinProgress, 6, 0);
   wclrtoeol(mWinProgress);
   std::stringstream ss3;
   ss3.precision(4);
   ss3 << "Time remaining " << (mLastTime - mStartTime)/mCurrDateIndex*mNumDates - (mLastTime - mStartTime) << "s       ";
   wprintw(mWinProgress, ss3.str().c_str());


   wrefresh(mWinProgress);
}
void LoggerNcurses::setCurrentLocationCore() {
   drawProgress();
   drawStatus();
}

void LoggerNcurses::drawStatus() {
   int width = getmaxx(mWinStatus);

   // Header
   wmove(mWinStatus, 0, 0);
   wattron(mWinStatus, COLOR_PAIR(3));
   wprintw(mWinStatus, mStatusHeader.c_str());
   wattroff(mWinStatus, COLOR_PAIR(3));

   // Legend
   std::stringstream ss;
   ss << "Cache name           Size (MB)     Cache misses   Cache usage";
   wmove(mWinStatus, 1, 0);
   wclrtoeol(mWinStatus);
   wprintw(mWinStatus, ss.str().c_str());

   int counter = 2;
   std::map<const std::string*, CacheInfo>::const_iterator it;
   for(it = mCaches.begin(); it  != mCaches.end(); it++) {
      std::string name = *(it->first);
      float size = *(it->second.mSize);
      float maxSize = *(it->second.mMaxSize);
      int misses    = *(it->second.mMisses);
      float trimSize = 0;
      std::stringstream ss;
      ss.width(20);
      ss << std::left << name;
      ss.precision(4);
      ss << std::left;
      if(size < 100000)
         ss << " < 0.1";
      else
         ss << " " << size/1024/1024;
      wmove(mWinStatus, counter, 0);
      wclrtoeol(mWinStatus);
      wprintw(mWinStatus, ss.str().c_str());

      {
         std::stringstream ss;
         ss << misses;
         wmove(mWinStatus, counter, 35);
         wclrtoeol(mWinStatus);
         wprintw(mWinStatus, ss.str().c_str());
      }


      // Draw progress bar
      if(!Global::isMissing(maxSize)) {
         wmove(mWinStatus, counter, 50);
         float y = 0;
         if(Global::isMissing(maxSize)) {
            y = 0;
         }
         else {
            if(trimSize > 0) {
               y = (maxSize - trimSize)/maxSize;
            }
            else {
               y = size/maxSize;
            }
         }
         //ss << " (" << maxSize/1024/1024<< ")";

         drawBar(mWinStatus, y, width-50, trimSize > 0);
      }
      counter++;
   }

   wrefresh(mWinStatus);
}

void LoggerNcurses::drawBar(WINDOW* iWin, float iFraction, int iMaxWidth, bool doRight) {
   std::stringstream ss;

   if(iFraction > 1) {
      iFraction = 1;
   }

   int numBars = (float) iFraction * (iMaxWidth-2);
   wprintw(iWin, "[");
   wattron(iWin, COLOR_PAIR(2)); 
   for(int i = 0; i < numBars; i++) {
      ss << "|";
   }
   wprintw(iWin, ss.str().c_str());
   wattroff(iWin, COLOR_PAIR(2)); 

   std::stringstream ss2;
   for(int i = numBars; i < iMaxWidth - 2; i++) {
      if(doRight) {
         ss2 << "|";
      }
      else {
         ss2 << " ";
      }
   }

   if(doRight) {
      wattron(iWin, COLOR_PAIR(1)); 
   }
   wprintw(iWin, ss2.str().c_str());
   if(doRight) {
      wattroff(iWin, COLOR_PAIR(1)); 
   }
   wprintw(iWin, "]");
}

void LoggerNcurses::setCurrentConfigurationCore() {
   //mStartTime = Global::clock(); // Reset starting clock

   int width = getmaxx(mWinConfig);

   // Header
   wmove(mWinConfig, 0, 0);
   wattron(mWinConfig, COLOR_PAIR(3));
   wprintw(mWinConfig, mConfigHeader.c_str());
   wattroff(mWinConfig, COLOR_PAIR(3));

   int currLine = 0;
   for(int i = 0; i < mConfigurations.size(); i++) {
      std::string name = mConfigurations[i]->getName();
      std::stringstream ss;

      if(i == mCurrConfiguration) {
         ss << " > ";
      }
      else {
         ss << "   ";
      }
      ss.width(width);
      ss << std::left << name;

      currLine++;
      wmove(mWinConfig, currLine, 0);
      if(i == mCurrConfiguration) {
         wattron(mWinConfig, COLOR_PAIR(2));
      }
      wprintw(mWinConfig, ss.str().c_str());
      if(i == mCurrConfiguration) {
         wattroff(mWinConfig, COLOR_PAIR(2));
      }

      // Draw components
      std::vector<const Component*> components;
      std::vector<Component::Type> types;
      mConfigurations[i]->getAllComponents(components, types);
      std::map<Component::Type, std::vector<const Component*> > componentMap;
      for(int c = 0; c < components.size(); c++) {
         componentMap[types[c]].push_back(components[c]);
      }
      std::map<Component::Type, std::vector<const Component*> >::const_iterator it;
      for(it = componentMap.begin(); it != componentMap.end(); it++) {
         std::string type = Component::getComponentName(it->first);
         std::vector<const Component*> components = it->second;
         for(int k = 0; k < (int) components.size(); k++) {
            std::stringstream ss;
            std::string name = components[k]->getSchemeName();
            //ss << type << ": " << name;
            ss << "   " << name;
            currLine++;
            wmove(mWinConfig, currLine, 0);
            wprintw(mWinConfig, ss.str().c_str());
         }
      }
      currLine++;

   }

   wrefresh(mWinConfig);

}

void LoggerNcurses::setCurrentDateCore() {
   double currTime = Global::clock();

   //mTimePerDate = mTimePerDate * (mTimeEfold-1)/mTimeEfold + (currTime - mLastTime)/mTimeEfold;
   mTimePerDate = currTime - mLastTime;
   mLastTime = currTime;
   drawProgress();
   drawStatus();
}
