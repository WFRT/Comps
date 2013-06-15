#ifndef DISCRETE_LOGIT_H
#define DISCRETE_LOGIT_H
#include "Discrete.h"
#include <boost/numeric/ublas/matrix.hpp>

class Distribution;
//! Probability is the fraction of ensemble members
class DiscreteLogit : public Discrete {
   public:
      DiscreteLogit(const Options& iOptions, const Data& iData);
      ~DiscreteLogit();
   protected:
      void updateParametersCore(const std::vector<Ensemble>& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const;
      void getVariables(const Ensemble& iEnsemble, std::vector<float>& iVariables) const;
      void getCoefficients(const Parameters& iParameters, Parameters& iCoefficients) const;
      void getDefaultParametersCore(Parameters& iParameters) const;
      float getPCore(const Ensemble& iEnsemble, const Parameters& iParameters) const;
   private:
      bool mUseConst;
      bool mUseMean;
      bool mUseFrac;
      int  mNumCoeff;
      float getLambda() const;
      bool getH(const Obs& iObs, const Ensemble& iEnsemble, const Parameters& iParameters, std::vector<float>& iH) const;
      bool getGradL(const Obs& iObs, const Ensemble& iEnsemble, const Parameters& iParameters, std::vector<float>& iGradL) const;
      static bool getInverse(boost::numeric::ublas::matrix<float> iMatrix, boost::numeric::ublas::matrix<float>& iInverse);

};
#endif
