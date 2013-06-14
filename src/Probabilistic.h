#ifndef PROBABILISTIC_H
#define PROBABILISTIC_H
#include "Component.h"
#include "Estimators/Estimator.h"

class Ensemble;
class Parameters;
class Obs;

class Probabilistic : public Component {
   public:
      Probabilistic(const Options& iOptions, const Data& iData);
      ~Probabilistic();
      virtual float getLikelihood(float iObs, const Ensemble& iEnsemble, const Parameters& iParameters) const = 0;
   protected:
      EstimatorProbabilistic* mEstimator;
};
#endif

