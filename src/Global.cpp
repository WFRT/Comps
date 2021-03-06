#include "Global.h"
#include "Loggers/None.h"
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/date_duration.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

//std::ofstream ofs("output.nl");
//Logger logger(std::ofstream("output.nl"));
float Global::INF = std::numeric_limits<float>::infinity();
float Global::MV  = -999;
float Global::NC  = -998;
float Global::TOL = 0.001;
float Global::pi  = 3.14159265;

boost::variate_generator<boost::mt19937, boost::uniform_01<> > Global::mRand(boost::mt19937(0), boost::uniform_01<>());

Logger* Global::logger = new LoggerNone(Logger::message);

void Global::setLogger(Logger* iLogger) {
   if(logger)
      delete logger;
   logger = iLogger;
}

bool Global::isValid(float iValue) {
   return !std::isnan(iValue) && !std::isinf(iValue) && !Global::isMissing(iValue);
}
bool Global::isMissing(float iValue) {
   return iValue == Global::MV;
}

float Global::getMoment(const std::vector<float>& iValues, int iMoment) {
   float moment = 0;
   // Find mean
   float mean = 0;
   int meanC = 0;
   for(int i = 0; i < (int) iValues.size(); i++) {
      assert(!std::isnan(iValues[i]));
      assert(!std::isinf(iValues[i]));
      if(!isMissing(iValues[i])) {
         mean += iValues[i];
         meanC++;
      }
   }
   if(meanC == 0) {
      return Global::MV;
   }

   mean = mean / meanC;

   if(iMoment == 1) {
      return mean;
   }
   else {
      // Compute central moment
      // TODO
      for(int i = 0; i < (int) iValues.size(); i++) {
         if(iValues[i] != Global::MV) {
            if(iMoment == 2)
               moment +=  (iValues[i] - mean)*(iValues[i] - mean);
            else
               moment += pow((double) iValues[i] - mean, (double) iMoment);
         }
      }
      if(meanC == 0)
         return Global::MV;
      else
         return moment / meanC;
   }
}
float Global::getTime(int iDate, int iInit, float iOffset) {
   float newOffset = iOffset + iInit;
   float time = getOffset(iDate, newOffset);
   assert(time >= 0 && time <= 24);
   return time;
}
int Global::getDate(int iDate, float iOffset) {
   return getDate(iDate, 0, iOffset);
}
int Global::getDayOfWeek(int iDate) {
   int year  = floor(iDate / 10000);
   int month = floor(iDate % 10000)/100;
   int day   = floor(iDate % 100);

   boost::gregorian::date currDate(year, month, day);
   return currDate.day_of_week();
}
int Global::getYear(int iDate) {
   return floor(iDate / 10000);
}
int Global::getMonth(int iDate) {
   return floor(iDate % 10000)/100;
}
int Global::getDay(int iDate) {
   return floor(iDate % 100);
}
int Global::getDate(int iDate, int iInit, float iOffset) {
   int year  = floor(iDate / 10000);
   int month = floor(iDate % 10000)/100;
   int day   = floor(iDate % 100);

   int offDay = floor((iOffset+iInit)/24);
   //std::cout << floor(2.5) << " " << ceil(2.5) << std::endl;
   //std::cout << floor(-2.5) << " " << ceil(-2.5) << std::endl;
   // Negative offsets round up, so subtract a day
   //if(iOffset < 0)
   //   offDay = iOffset - 1;

   boost::gregorian::date currDate(year, month, day);
   boost::gregorian::date_duration diff(offDay);
   boost::gregorian::date newDate = currDate + diff; 
   int newYear  = newDate.year();
   int newMonth = newDate.month();
   int newDay   = newDate.day();
   int returnDate = newYear * 10000 + newMonth * 100 + newDay;
   return returnDate;
}

float Global::getOffset(int iDate, float iOffset) {
   float offset;
   if(iOffset < 0) {
      int addDay = -floor(iOffset/24);
      offset = iOffset + 24*addDay;
   }
   else {
      offset = fmod(iOffset, 24);
   }
   assert(offset < 24);
   assert(offset >= 0);
   return offset;
}

float Global::getTimeDiff(int iDate1, int iInit1, float iOffset1, int iDate2, int iInit2, float iOffset2) {
   int year1 = floor(iDate1 / 10000);
   int month1= floor(iDate1 % 10000)/100;
   int day1  = floor(iDate1 % 100);

   int year2 = floor(iDate2 / 10000);
   int month2= floor(iDate2 % 10000)/100;
   int day2  = floor(iDate2 % 100);

   boost::gregorian::date date1(year1, month1, day1);
   boost::gregorian::date date2(year2, month2, day2);
   boost::gregorian::date_period diff(date2, date1);
   long int numDays = diff.length().days();

   return numDays*24 + (iInit1 - iInit2) + (iOffset1 - iOffset2);
}

float Global::getJulianDay(int iDate, int iInit, float iOffset) {
   int year  = floor(iDate / 10000);
   int month = floor(iDate % 10000)/100;
   int day   = floor(iDate % 100);

   boost::gregorian::date currDate(year, month, day);
   return currDate.day_of_year();

   /*
   struct tm tm1;

   tm1.tm_year  = year-1900;
   tm1.tm_mon   = month-1;
   tm1.tm_mday  = day;
   tm1.tm_hour  = 0;
   tm1.tm_min   = 0;
   tm1.tm_sec   = 0;
   tm1.tm_isdst = 0;

   time_t time = mktime(&tm1) + iOffset/60;

   struct tm *tm2 = gmtime(&time);

   return tm2->tm_yday;
   */

}
double Global::clock() {
   timeval t;
   gettimeofday(&t, NULL);
   double sec = (t.tv_sec);
   double msec= (t.tv_usec);
   return sec + msec/1e6;
}

int Global::getLowerIndex(float iX, const std::vector<float>& iValues) {
   int index = Global::MV;
   for(int i = 0; i < (int) iValues.size(); i++) {
      float currValue = iValues[i];
      if(Global::isValid(currValue)) {
         if(currValue < iX) {
            index = i;
         }
         else if(currValue == iX) {
            index = i;
            break;
         }
         else if(currValue > iX) {
            break;
         }
      }
   }
   return index;
}
int Global::getUpperIndex(float iX, const std::vector<float>& iValues) {
   int index = Global::MV;
   for(int i = iValues.size()-1; i >= 0; i--) {
      float currValue = iValues[i];
      if(Global::isValid(currValue)) {
         if(currValue > iX) {
            index = i;
         }
         else if(currValue == iX) {
            index = i;
            break;
         }
         else if(currValue < iX) {
            break;
         }
      }
   }
   return index;
}
int Global::getNumValid(std::vector<float> iValues) {
   int num = 0;
   for(int i = 0; i < iValues.size(); i++) {
      if(Global::isValid(iValues[i])) {
         num++;
      }
   }
   return num;
}

float Global::interp(float x, float x0, float x1, float y0, float y1) {
   if(isValid(x) && isValid(x0) && isValid(x1) && isValid(y0) && isValid(y1)) {
      if(x0 == x1) {
         return (y1 + y0)/2;
      }
      else {
         assert(x >=x0);
         assert(x <=x1);
         assert(x1 != x0);
         return y0 + (y1 - y0)*(x - x0)/(x1 - x0);
      }
   }
   else {
      return Global::MV;
   }
}
float Global::getRand() {
   return mRand();
}

int Global::getCurrentDate() {
    boost::gregorian::date today = boost::gregorian::day_clock::local_day();
    return today.year()*10000 + today.month()*100 + today.day();
}

int Global::getInt(const std::string& iString) {
   std::stringstream ss(iString);
   int value;
   ss >> value;
   return(value);
}
float Global::getFloat(const std::string& iString) {
   std::stringstream ss(iString);
   float value;
   ss >> value;
   return(value);
}

bool Global::createDirectory(std::string iPath) {
   if(!boost::filesystem::exists(iPath)) {
      if(!boost::filesystem::create_directory(iPath)) {
         return false;
      }
   }
   return true;
}

std::string Global::getDirectory(std::string iFilename) {
   int last = iFilename.find_last_of("/");
   if(last != std::string::npos) {
      std::string dir = iFilename.substr(0, last);
      return dir;
   }
   else {
      return "./";
   }
}

int Global::getDate(time_t iUnixTime) {
   boost::gregorian::date epoch(1970,1,1);
   boost::gregorian::date_duration diff(iUnixTime/86400);
   boost::gregorian::date newDate = epoch + diff;

   return newDate.year() * 10000 + newDate.month() * 100 + newDate.day();
}
float Global::getTime(time_t iUnixTime) {
   int date = getDate(iUnixTime);
   return (iUnixTime - getUnixTime(date, 0))/3600;
}
time_t Global::getUnixTime(int iDate, int iOffset) {
   int   date   = getDate(iDate, iOffset);
   float offset = getOffset(iDate, iOffset);
   int year = getYear(date);
   int month = getMonth(date);
   int day = getDay(date);
   // boost::posix_time::ptime unixTime(boost::gregorian::date(year, month, day));
   boost::gregorian::date time(year, month, day);
   boost::gregorian::date epoch(1970,1,1);
   boost::gregorian::date_duration diff = time - epoch;
   time_t days = diff.days();
   time_t unixTime = days*86400 + ((time_t) offset)*3600;
   return unixTime;
}
float Global::logit(float p) {
   if(!Global::isValid(p) || p <= 0 || p >= 1)
      return Global::MV;
   return log(p/(1-p));
}
float Global::invLogit(float x) {
   if(!Global::isValid(x))
      return Global::MV;
   return exp(x)/(exp(x)+1);
}
