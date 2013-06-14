#ifndef DISCRETE_ZERO_H
#define DISCRETE_ZERO_H
#include "Discrete.h"

class Distribution;
//! Probability is the fraction of ensemble members
class DiscreteZero : public Discrete {
   public:
      DiscreteZero(const Options& iOptions, const Data& iData);
      float getPCore(const Ensemble& iEnsemble, const Parameters& iParameters) const {return 0;};
      bool isMemberDependent() const {return false;};
      bool isLocationDependent() const {return false;};
      bool needsTraining() const {return false;};
      bool isVariableDependent() const {return false;}; 
   protected:
      void updateParametersCore(
            const Ensemble& iEnsemble,
            const Obs& iObs,
            Parameters& iParameters) const {return;};
};
#endif
