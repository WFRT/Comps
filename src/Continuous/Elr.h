#ifndef CONTINUOUS_ELR_H
#define CONTINUOUS_ELR_H
#include "Continuous.h"

class Transform;

//! Extended logistic regression with the equation:
//! log[Pr(X > x)/Pr(X < x)] = a - f(ensMean) + b * f(x),
//! where f is a transform, and a and b are parameters that are estimated
class ContinuousElr : public Continuous {
   public:
      ContinuousElr(const Options& iOptions, const Data& iData);
      ~ContinuousElr();
   protected:
      float getCdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      float getPdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      float getInvCore(float iCdf, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      void getDefaultParametersCore(Parameters& iParameters) const;
      void updateParametersCore(const std::vector<Ensemble>& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const;
      float getLogit(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      Transform* mTransform;
};
#endif
