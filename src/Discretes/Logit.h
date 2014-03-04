#ifndef DISCRETE_LOGIT_H
#define DISCRETE_LOGIT_H
#include "Discrete.h"
#include <boost/numeric/ublas/matrix.hpp>

class Distribution;
class Measure;
//! Discrete probability using logistic regression. Probability is of the form: logit(P) = a + b * m
//! + c * f, where a, b, c are constants, m is the ensemble mean, and f is the fraction of ensemble
//! members lying on the discrete point.
class DiscreteLogit : public Discrete {
   public:
      DiscreteLogit(const Options& iOptions, const Data& iData);
   private:
      void getDefaultParametersCore(Parameters& iParameters) const;
      int  numParametersCore() const;
      void updateParametersCore(const std::vector<Ensemble>& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const;
      void getVariables(const Ensemble& iEnsemble, std::vector<float>& iVariables) const;
      void getCoefficients(const Parameters& iParameters, Parameters& iCoefficients) const;
      float getPCore(const Ensemble& iEnsemble, const Parameters& iParameters) const;
      std::vector<Measure*> mMeasures;
      float getLambda() const;
      bool getH(const Obs& iObs, const Ensemble& iEnsemble, const Parameters& iParameters, std::vector<float>& iH) const;
      bool getGradL(const Obs& iObs, const Ensemble& iEnsemble, const Parameters& iParameters, std::vector<float>& iGradL) const;
      static bool getInverse(boost::numeric::ublas::matrix<float> iMatrix, boost::numeric::ublas::matrix<float>& iInverse);

      bool mUseConst;
      int  mNumCoeff;
};
#endif
