#include "DiscreteOnly.h"
#include "../Parameters.h"
#include "../Options.h"
#include "../Scheme.h"
#include "../Continuous/Continuous.h"
#include "../Discretes/Discrete.h"
#include "../Distribution.h"
#include "../Variables/Variable.h"
#include "../Obs.h"

UncertaintyDiscreteOnly::UncertaintyDiscreteOnly(const Options& iOptions, const Data& iData) : Uncertainty(iOptions, iData) {
   std::string tag;
   iOptions.getRequiredValue("discrete", tag);
   Options opt;
   Scheme::getOptions(tag, opt);
   mDiscrete = Discrete::getScheme(opt, iData);
}
UncertaintyDiscreteOnly::~UncertaintyDiscreteOnly() {
   delete mDiscrete;
}

float UncertaintyDiscreteOnly::getCdf(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   float cdf = mDiscrete->getP(iEnsemble, iParameters);
   if(!Global::isValid(cdf)) {
      return Global::MV;
   }
   return cdf;
}
float UncertaintyDiscreteOnly::getPdf(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   return getCdf(iX, iEnsemble, iParameters);
}
float UncertaintyDiscreteOnly::getInv(float iCdf, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   float P = mDiscrete->getP(iEnsemble, iParameters);
   return iCdf > P;
}
float UncertaintyDiscreteOnly::getMoment(int iMoment, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   return mDiscrete->getX();
}

void UncertaintyDiscreteOnly::updateParameters(const std::vector<Ensemble>& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const {
   mDiscrete->updateParameters(iEnsemble, iObs, iParameters);
}
void UncertaintyDiscreteOnly::getDefaultParameters(Parameters& iParameters) const {
   mDiscrete->getDefaultParameters(iParameters);
}
bool UncertaintyDiscreteOnly::needsConstantEnsembleSize() const {
   return mDiscrete->needsConstantEnsembleSize();
}
