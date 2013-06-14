#ifndef TIMESTAMP_H
#define TIMESTAMP_H
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/date_duration.hpp>
//#include <boost/date_time/gregorian/greg_serialize.hpp>
#include "Global.h"

class TimeStamp {
   public:
      TimeStamp(int rDate, int init, float rOffset);
      int getTime() const;
      int getMonth() const;
      int getYear() const;
      int getDay() const;
      int getHour() const;
      int getMinute() const;
      int getSecond() const;
      boost::gregorian::date getDate() const;
      boost::gregorian::date_duration getDiff(const TimeStamp& rOther) const;
   private:
      boost::gregorian::date mDate;
      //int mDate;
      int mInit;
      float mOffset;
      //int mTime;
};
#endif

