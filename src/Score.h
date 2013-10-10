#ifndef SCORE_H
#define SCORE_H
#include "Entity.h"

class Score : public Entity {
   public:
      Score();
      Score(std::string iMetric, float iValue, int iDate, int iInit, float iOffset, const Location& iLocation, const std::string& iVariable);
      std::string getMetric() const {return mMetric;};
      float getValue() const {return mValue;};
   private:
      std::string mMetric;
      float mValue;
};
#endif
