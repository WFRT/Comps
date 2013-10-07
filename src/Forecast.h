#ifndef FORECAST_H
#define FORECAST_H

class Forecast {
   public:
      Forecast();
      Value getDeterministic();
      Ensemble getEnsemble()
      Distribution::ptr getDistribution()
      int getDate();
      Location getLocation();
      float getOffset();
      int getInit();
      std::string getVariable();
   private:
      Value mdeterministic;
      Ensemble mEnsemble;
      Distribution::ptr mDistribution;
      Location mLocation;
      int mDate;
      float mOffset;
      int mInit;
      std::string mVariable;
};
#endif

