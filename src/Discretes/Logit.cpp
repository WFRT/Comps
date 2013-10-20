#include "Logit.h"
#include "../Obs.h"
#include "../Scheme.h"
#include "../Parameters.h"
#include "../Variables/Variable.h"
#include "../Measures/Measure.h"
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

DiscreteLogit::DiscreteLogit(const Options& iOptions, const Data& iData) : Discrete(iOptions, iData),
      mUseConst(false) {

   //! Include the constant term in the regression?
   iOptions.getValue("useConst", mUseConst);
   std::vector<std::string> measureTags;

   //! Ensemble measures to use in regression model
   iOptions.getValues("measures", measureTags);
   for(int i = 0; i < measureTags.size(); i++) {
      Measure* measure = Measure::getScheme(measureTags[i], mData);
      mMeasures.push_back(measure);
   }

   mNumCoeff = mUseConst + mMeasures.size();
   if(mNumCoeff == 0) {
      std::stringstream ss;
      ss << "DiscreteLogit: At least one of 'useConst' and 'measures' must selected";
      Global::logger->write(ss.str(), Logger::error);
   }

   if(mEstimator == NULL) {
      std::stringstream ss;
      ss << "DiscreteLogit: No estimator specified. Default to EstimatorMaximumLikelihood.";
      Global::logger->write(ss.str(), Logger::message);
      Options opt("tag=test class=EstimatorMaximumLikelihood");
      mEstimator = EstimatorProbabilistic::getScheme(opt, iData, *this);
   }
}

float DiscreteLogit::getPCore(const Ensemble& iEnsemble, const Parameters& iParameters) const {
   const Variable* var = Variable::get(iEnsemble.getVariable());

   std::vector<float> vars;
   getVariables(iEnsemble, vars);

   if(mEstimator) {
      assert(vars.size() == iParameters.size());
   }
   else {
      assert((int) vars.size() + vars.size()*vars.size() == iParameters.size());
   }

   // Add up regression terms
   float logit = 0;
   for(int i = 0; i < mNumCoeff; i++) {
      if(!Global::isValid(iParameters[i]) || !Global::isValid(vars[i])) {
         return Global::MV;
      }
      logit += iParameters[i] * vars[i];
   }

   assert(Global::isValid(logit));
   float P = exp(logit)/(1 + exp(logit));

   assert(P>0 && P < 1);

   return P;
}

void DiscreteLogit::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> param(mNumCoeff,0);
   iParameters.setAllParameters(param);
}

void DiscreteLogit::getVariables(const Ensemble& iEnsemble, std::vector<float>& iVariables) const {
   const Variable* var = Variable::get(iEnsemble.getVariable());

   iVariables.clear();
   float logit = 0;
   int counter = 0;
   if(mUseConst) {
      iVariables.push_back(1);
   }
   for(int i = 0; i < mMeasures.size(); i++) {
      // TODO: Parameters
      float measure = mMeasures[i]->measure(iEnsemble);
      iVariables.push_back(measure);
   }
   assert((int) iVariables.size() == mNumCoeff);
}
void DiscreteLogit::getCoefficients(const Parameters& iParameters, Parameters& iCoefficients) const {
   std::vector<float> param;
   param.resize(mNumCoeff);

   assert(iParameters.size() == mNumCoeff + mNumCoeff*mNumCoeff);
   for(int i = 0; i < mNumCoeff; i++) {
      param[i] = iParameters[i];
   }
   iCoefficients.setAllParameters(param);
}
void DiscreteLogit::updateParametersCore(const std::vector<Ensemble>& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const {
   std::stringstream ss;
   ss << "DiscreteLogit: Does not have its own parameter estimator. Must use external estimator";
   Global::logger->write(ss.str(), Logger::error);
}

float DiscreteLogit::getLambda() const {
   assert(Global::isValid(mEfold));
   return 1 - 1/mEfold;
}

bool DiscreteLogit::getH(const Obs& iObs, const Ensemble& iEnsemble, const Parameters& iParameters, std::vector<float>& iH) const {

   iH.clear();
   iH.resize(mNumCoeff);

   std::vector<float> gradL;
   bool status = getGradL(iObs, iEnsemble, iParameters, gradL);
   if(!status) {
      return false;
   }
   //std::cout << "Grad = " << gradL[0] << std::endl;
   assert((int) gradL.size() == mNumCoeff);

   float L = getLikelihood(iObs.getValue(), iEnsemble, iParameters);
   assert(L > 0);
   if(!Global::isValid(L)) {
      return false;
   }
   for(int i = 0; i < mNumCoeff; i++) {
      float value = gradL[i]/L;
      iH[i] = value;
   }
   return true;
}

bool DiscreteLogit::getGradL(const Obs& iObs, const Ensemble& iEnsemble, const Parameters& iParameters, std::vector<float>& iGradL) const {

   iGradL.clear();
   iGradL.resize(mNumCoeff);

   Parameters coeffs;
   getCoefficients(iParameters, coeffs);

   float dx = 0.001;
   float L0 = getLikelihood(iObs.getValue(), iEnsemble, iParameters);
   if(!Global::isValid(L0))
      return false;
   for(int i = 0; i < mNumCoeff; i++) {
      // Perturb current parameter
      Parameters par = iParameters;
      par[i] += dx;

      // Compute likelihood derivative
      float currL = getLikelihood(iObs.getValue(), iEnsemble, par);
      if(!Global::isValid(currL))
         return false;
      float dL = currL - L0;
      iGradL[i] = dL/dx;
   }
   return true;
}

bool DiscreteLogit::getInverse(boost::numeric::ublas::matrix<float> iMatrix, boost::numeric::ublas::matrix<float>& iInverse) {
   // Taken from https://gist.github.com/2464434
   using namespace boost::numeric::ublas;
   typedef permutation_matrix<std::size_t> pmatrix;
   // create a working copy of the input
   matrix<float> A(iMatrix);
   // create a permutation matrix for the LU-factorization
   pmatrix pm(A.size1());

   // perform LU-factorization
   int res = lu_factorize(A,pm);
   if( res != 0 ) return false;

   // create identity matrix of "inverse"
   iInverse.assign(identity_matrix<float>(A.size1()));

   // backsubstitute to get the inverse
   lu_substitute(A, pm, iInverse);

   return true;
}
