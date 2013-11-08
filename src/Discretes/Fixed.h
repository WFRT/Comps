#ifndef DISCRETE_FIXED_H
#define DISCRETE_FIXED_H
#include "Discrete.h"

class Distribution;
//! Probability is fixed to a value.
class DiscreteFixed : public Discrete {
   public:
      DiscreteFixed(const Options& iOptions, const Data& iData);
      float getPCore(const Ensemble& iEnsemble, const Parameters& iParameters) const {return mValue;};
      bool isMemberDependent() const {return false;};
      bool isLocationDependent() const {return false;};
      bool needsTraining() const {return false;};
      bool isVariableDependent() const {return false;}; 
   protected:
      void updateParametersCore(const std::vector<Ensemble>& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const {};
      float mValue;
};
#endif
