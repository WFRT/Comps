#ifndef ESTIMATOR_MAXIMUM_LIKELIHOOD_H
#define ESTIMATOR_MAXIMUM_LIKELIHOOD_H
#include "Estimator.h"
#include <boost/numeric/ublas/matrix.hpp>
class Ensemble;

class EstimatorMaximumLikelihood : public EstimatorProbabilistic {
   public:
      EstimatorMaximumLikelihood(const Options& iOptions, const Data& iData, const Probabilistic& iScheme);
      void update(const std::vector<Ensemble>& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const;
   private:
      static int getIndex(int i, int j, int iSize);
      float getLambda() const;
      bool getH(float iObs, const Ensemble& iEnsemble, const Parameters& iCoeffs, std::vector<float>& iH) const;
      bool getGradL(float iObs, const Ensemble& iEnsemble, const Parameters& iCoeffs, std::vector<float>& iGradL) const;
      static bool getInverse(boost::numeric::ublas::matrix<float> iMatrix, boost::numeric::ublas::matrix<float>& iInverse);
      void getDefaultParameters(const Parameters& iSchemeParameters, Parameters& iParameters) const;
      int getNumCoefficients(const Parameters& iParameters) const;
};
#endif

