#ifndef AVERAGER_BEST_H
#define AVERAGER_BEST_H
#include "../Global.h"
#include "../Options.h"

//! Use the ensemble member that has the lowest mean absolute error
//! Will not work if the ensemble size varies from day to day
class AveragerBest : public Averager {
   public:
      AveragerBest(const Options& iOptions, const Data& iData);
      float average(const Ensemble& iValues, const Parameters& iParameters) const;
      void updateParameters(const std::vector<Ensemble>& iValues,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
      void getDefaultParameters(Parameters& iParameters) const;
      bool needsTraining() const {return true;};
   private:
      int numParametersCore() const {return 1;};
};
#endif
