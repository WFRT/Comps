#include "Elr.h"
#include "../Obs.h"
#include "../BaseDistributions/BaseDistribution.h"
#include "../Scheme.h"
#include "../Parameters.h"
#include "../Measures/Measure.h"

ContinuousElr::ContinuousElr(const Options& iOptions, const Data& iData) :
      Continuous(iOptions, iData), mTransform(NULL) {
   std::string transformTag;
   //! What transform should be applied to the ensemble mean?
   if(iOptions.getValue("transform", transformTag)) {
      mTransform = Transform::getScheme(transformTag);
   }
   iOptions.check();
}
ContinuousElr::~ContinuousElr() {
   if(mTransform)
      delete mTransform;
}

float ContinuousElr::getCdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   float logit = getLogit(iX, iEnsemble, iParameters);
   if(!Global::isValid(logit))
      return Global::MV;
   float cdf   = 1 / (1 + exp(-logit));

   return cdf;
}
float ContinuousElr::getPdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   float logit = getLogit(iX, iEnsemble, iParameters);
   if(!Global::isValid(logit))
      return Global::MV;
   float b = iParameters[0];
   float pdf   = -pow(1+ exp(-logit),-2)*exp(-logit)*(-b);
   if(mTransform)
      pdf = pdf * mTransform->derivative(iX);

   return pdf;
}

void ContinuousElr::updateParametersCore(const std::vector<Ensemble>& iEnsemble,
      const std::vector<Obs>& iObs, Parameters& iParameters) const {
   std::stringstream ss;
   ss << "ContinousElr: Does not have its own parameter estimator. Must use external estimator";
   Global::logger->write(ss.str(), Logger::error);
}

void ContinuousElr::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> param(1,Global::MV);
   param[0] = 1; // Bias
   // param[1] = 1; // Variance
   if(mTransform)
      param[0] = mTransform->transform(param[0]);
   iParameters.setAllParameters(param);
}

float ContinuousElr::getInvCore(float iCdf, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   float ens = iEnsemble.getMoment(1);
   if(!Global::isValid(ens))
      return Global::MV;

   if(mTransform)
      ens = mTransform->transform(ens);

   //assert(iParameters.size() == 2);
   //float a = iParameters[0];
   //float b = iParameters[1];
   //float x = (-log(1 / iCdf - 1) - a + ens) / b;

   float a = 0;
   float b = iParameters[0];
   float logit = -log(1 / iCdf - 1);
   float x = (logit - a + ens) / b;
   //float x = (-log(1 / iCdf - 1) - a + ens) / b;
   if(mTransform)
      x = mTransform->inverse(x);

   return x;
}

float ContinuousElr::getLogit(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   float ens = iEnsemble.getMoment(1);
   if(!Global::isValid(ens))
      return Global::MV;

   if(mTransform) {
      ens = mTransform->transform(ens);
      iX = mTransform->transform(iX);
   }
   assert(iParameters.size() == 1);
   float a = 0; //iParameters[0];
   float b = iParameters[0];
   float logit = a - ens + b * iX;
   return logit;
}
