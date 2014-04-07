#include "Discrete.h"
#include "SchemesHeader.inc"
#include "../Variables/Variable.h"
#include "../Obs.h"
#include "../Scheme.h"
#include "../Parameters.h"

float Discrete::mMinP = 1e-7;
Discrete::Discrete(const Options& iOptions, const Data& iData) : Probabilistic(iOptions, iData) {
   mType = Component::TypeDiscrete;

   //! The value of the variable that the discrete mass is located at
   iOptions.getRequiredValue("x", mX);
}
#include "Schemes.inc"

void Discrete::updateParameters(
      const std::vector<Ensemble>& iEnsemble,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   if(mEstimator) {
      mEstimator->update(iEnsemble, iObs, iParameters);
   }
   else {
      updateParametersCore(iEnsemble, iObs, iParameters);
   }
}

void Discrete::updateParametersCore(
      const std::vector<Ensemble>& iEnsemble,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   std::stringstream ss;
   ss << "This scheme: " << typeid(*this).name() << " requires an estimator" << std::endl;
   Global::logger->write(ss.str(), Logger::error);
}

float Discrete::getLikelihood(float iObs, const Ensemble& iEnsemble, const Parameters& iParameters) const {

   float P = getPCore(iEnsemble, iParameters);
   float value;
   if(iObs == mX) {
      value = P;
   }
   else {
      value = 1 - P;
   }
   if(value == 0) {
      Global::logger->write("Warning: likelihood=0, reset to small non-zero value.", Logger::warning);
      value = Discrete::mMinP;
   }
   if(value == 1) {
      Global::logger->write("Warning: likelihood=1, reset to value just below 1.", Logger::warning);
      value = 1-Discrete::mMinP;
   }
   return value;
}


void Discrete::getDefaultParameters(Parameters& iParameters) const {
   if(mEstimator) {
      Parameters parScheme;
      getDefaultParametersCore(parScheme);
      Parameters parEstimator;
      mEstimator->getDefaultParameters(parScheme, parEstimator);

      // Combine parameters
      std::vector<float> param;
      int Ns = (int) parScheme.size();
      int Ne = (int) parEstimator.size();
      param.resize(Ns + Ne);
      for(int i = 0; i < Ns;  i++) {
         param[i] = parScheme[i];
      }
      for(int i = 0; i < Ne; i++) {
         param[Ns + i] = parEstimator[i];
      }
      iParameters.setAllParameters(param);
   }
   else {
      getDefaultParametersCore(iParameters);
   }

}

float Discrete::getP(const Ensemble& iEnsemble, const Parameters& iParameters) const {
   if(mEstimator) {
      // Remove estimator parameters
      Parameters coeffs;
      mEstimator->getCoefficients(iParameters, coeffs);
      return getPCore(iEnsemble, coeffs);

   }
   else {
      // TODO:
      //Parameters coeffs;
      //mEstimator->getCoefficients(iParameters, coeffs);
      return getPCore(iEnsemble, iParameters);
   }
}

float Discrete::getX() const {
   return mX;
}
int Discrete::numParameters() const {
   int numEstParameters = 0;
   if(mEstimator != NULL) {
      numEstParameters = mEstimator->numParameters();
      if(!Global::isValid(numEstParameters))
         return Global::MV;
   }
   int numSchemeParameters = numParametersCore();
   if(Global::isValid(numSchemeParameters))
      return numEstParameters + numSchemeParameters;
   else
      return Global::MV;
}
