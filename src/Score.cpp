#include "Score.h"

Score::Score() :
      mValue(Global::MV),
      mMetric("") {

}

Score::Score(std::string iMetric, float iValue, int iDate, int iInit, float iOffset, const Location& iLocation, const std::string& iVariable) :
      Entity(iDate, iInit, iOffset, iLocation, iVariable), 
      mValue(iValue), mMetric(iMetric) {

}
