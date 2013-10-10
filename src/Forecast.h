#ifndef FORECAST_H
#define FORECAST_H
#include "Deterministic.h";
#include "Ensemble.h";
#include "Distribution.h"
#include "Location.h";

class Forecast : public Entity {
   public:
      Forecast(const Deterministic& iDeterministic, const Ensemble& iEnsemble, const Distribution::ptr iDistribution);
      // This should be done by configuration
      // Forecast(int iDate, int iInit, float iOffset, const Location& iLocation, std::string iVariable, const Configuration& iConfiguration);
      Deterministic getDeterministic() const {return mDeterministic;};
      Ensemble getEnsemble() const {return mEnsemble;};
      Distribution::ptr getDistribution() const {return mDistribution;};
      int getDate() const {return mEnsemble.getDate();}; 
      Location getLocation() const {return mEnsemble.getLocation();};
      float getOffset() const {return mEnsemble.getOffset();};
      int getInit() const {return mEnsemble.getInit();};
      std::string getVariable() const {return mEnsemble.getVariable();};
   private:
      Deterministic mDeterministic;
      Ensemble mEnsemble;
      Distribution::ptr mDistribution;
      int mDate;
      float mOffset;
      int mInit;
      Location mLocation;
      std::string mVariable;
};
#endif

