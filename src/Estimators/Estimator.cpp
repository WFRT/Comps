#include "Estimator.h"
#include "SchemesHeader.inc"
#include "../Parameters.h"

Estimator::Estimator(const Options& iOptions, const Data& iData) :
      Component(iOptions, iData) {};
//EstimatorProbabilistic::EstimatorProbabilistic(const Options& iOptions, const Data& iData) : Estimator(iOptions, iData) {}
//#include "Schemes.inc"

EstimatorProbabilistic::EstimatorProbabilistic(const Options& iOptions, const Data& iData, const Probabilistic& iScheme) :
      Estimator(iOptions, iData),
      mScheme(iScheme) {}

void Estimator::getDefaultParameters(Parameters& iParameters) {
   Global::logger->write("Estimator cannot give default parameters without specifying a probabilistic instance", Logger::error);
}

// TODO: Should be auto-generated
EstimatorProbabilistic* EstimatorProbabilistic::getScheme(const Options& iOptions, const Data& iData, const Probabilistic& iScheme) {
   std::string className;
   iOptions.getRequiredValue("class", className);
   if(0) {}
   else if(className == "EstimatorMaximumLikelihood") {
       return new EstimatorMaximumLikelihood(iOptions, iData, iScheme);
   }
   else {
      Component::doesNotExist(className);
   }
}

void EstimatorProbabilistic::getCoefficients(const Parameters& iParameters, Parameters& iCoefficients) const {
   int N = getNumCoefficients(iParameters);
   std::vector<float> param;
   param.resize(N);

   assert(iParameters.size() == N + N*N);
   // Only take the first N parameters
   for(int i = 0; i < N; i++) {
      param[i] = iParameters[i];
   }
   iCoefficients.setAllParameters(param);
}
