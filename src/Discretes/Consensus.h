#ifndef DISCRETE_MOMENTS_H
#define DISCRETE_MOMENTS_H
#include "Discrete.h"

class Distribution;
//! Probability is the fraction of ensemble members forecasting the boundary.
//! Assumptions: the obs is equally likely to fall between any consecutive ensemble members
//! When one of two consecutive members fall on the boundary, half the mass is placed on the boundary
//! the other half in the continuous part.
class DiscreteConsensus : public Discrete {
   public:
      DiscreteConsensus(const Options& iOptions, const Data& iData);
      bool needsTraining() const {return false;};
   private:
      float getPCore(const Ensemble& iEnsemble, const Parameters& iParameters) const;
};
#endif
