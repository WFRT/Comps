#include "TimeStamp.h"

TimeStamp::TimeStamp() {
}
TimeStamp::TimeStamp(int rDate, int rInit, float rOffset) :
      mDate(rDate),
      mInit(rInit),
      mOffset(rOffset) {
   int year  = floor(rDate / 10000);
   int month = floor((rDate % 10000) / 100);
   int day   = rDate % 100;
   mDate = boost::gregorian::date(year, month, day);
}
boost::gregorian::date TimeStamp::getDate() const {
   return mDate;
}
int TimeStamp::getTime() const {
   return 0;
}
boost::gregorian::date_duration TimeStamp::getDiff(const TimeStamp& rOther) const {
   return (mDate - rOther.getDate());
}

int TimeStamp::getYear() const {

}
int TimeStamp::getMonth() const {

}
int TimeStamp::getDay() const {

}
int TimeStamp::getHour() const {

}
int TimeStamp::getMinute() const {
}
int TimeStamp::getSecond() const {
   return 
}
