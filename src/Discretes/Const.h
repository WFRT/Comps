#ifndef DISCRETE_CONST_H
#define DISCRETE_CONST_H
#include "Discrete.h"

class Distribution;
//! Probability is a value independent of the ensemble. It is adjusted based on the fraction of past
//! observations falling on the discrete point.
class DiscreteConst : public Discrete {
   public:
      DiscreteConst(const Options& iOptions, const Data& iData);
      float getPCore(const Ensemble& iEnsemble, const Parameters& iParameters) const;
   private:
      void getDefaultParametersCore(Parameters& iParameters) const;
      int numParametersCore() const {return 1;};
      void updateParametersCore(const std::vector<Ensemble>& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const;
      float mInitialP;
};
#endif
