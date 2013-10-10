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
      Deterministic     getDeterministic() const {return mDeterministic;};
      Ensemble          getEnsemble() const {return mEnsemble;};
      Distribution::ptr getDistribution() const {return mDistribution;};
   private:
      Deterministic     mDeterministic;
      Ensemble          mEnsemble;
      Distribution::ptr mDistribution;
};
#endif

