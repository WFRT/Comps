#ifndef SCORE_H
#define SCORE_H

class Score : Value {
   public:
      Score();
      Score(Metric iMetric, float iValue, int iDate, int iInit, float iOffset, const Location& iLocation, const std::string& iVariable, Input::Type iType=Input::typeForecast);
      Metric getMetric() const {return mMetric;};
   private:
      Metric mMetric;
};
#endif
