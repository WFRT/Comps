#include "Gamma.h"
#include "../BaseDistributions/BaseDistribution.h"
#include "../Scheme.h"
#include "../Variables/Variable.h"
#include "../Obs.h"
ContinuousGamma::ContinuousGamma(const Options& iOptions, const Data& iData) : Continuous(iOptions, iData) {
   /*
   std::string distributionTag;
   iOptions.getRequiredValue("distribution", distributionTag);
   Options optDistribution;
   Scheme::getOptions(distributionTag, optDistribution);
   */
   Options optDistribution("tag=gamma class=BaseDistributionGamma");
   mBaseDistribution = BaseDistribution::getScheme(optDistribution, iData);
   iOptions.check();
}

ContinuousGamma::~ContinuousGamma() {
   delete mBaseDistribution;
}

float ContinuousGamma::getCdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {

   std::vector<float> moments;
   if(getMoments(iEnsemble, iParameters, moments)) {
      return mBaseDistribution->getCdf(iX, moments);
   }
   else {
      return Global::MV;
   }
}
float ContinuousGamma::getPdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {

   std::vector<float> moments;
   if(getMoments(iEnsemble, iParameters, moments)) {
      return mBaseDistribution->getPdf(iX, moments);
   }
   else {
      return Global::MV;
   }
}
float ContinuousGamma::getInvCore(float iCdf, const Ensemble& iEnsemble, const Parameters& iParameters) const {

   std::vector<float> moments;
   if(getMoments(iEnsemble, iParameters, moments)) {
      return mBaseDistribution->getInv(iCdf, moments);
   }
   else {
      return Global::MV;
   }
}

void ContinuousGamma::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> param;
   // This parameter measures the average obs value when the ensemble mean is on the lower boundary
   //float meanAdjust = 1;
   //param.push_back(meanAdjust);

   // mu = c0 + c1 * cubeRoot(e)
   param.push_back(2); // c0
   param.push_back(2); // c1
   // sigma = c2 * mu
   //param.push_back(4); // c2
   //param.push_back(1); // c3

   iParameters.setAllParameters(param);
}
/*
void ContinuousGamma::updateParametersCore(const Ensemble& iEnsemble, const Obs& iObs, Parameters& iParameters) const {

   float oldMean = iParameters[0];

   float ensMean = Global::mean(iEnsemble.getValues());

   std::string variable = iEnsemble.getVariable();
   float min = Variable::get(variable)->getMin();
   float obs = iObs.getValue();
   if(ensMean == min && Global::isValid(obs)) {
      float newMean = obs;
      iParameters[0] = Processor::combine(oldMean, newMean);
   }
   return;
}
*/

bool ContinuousGamma::getMoments(const Ensemble& iEnsemble, const Parameters& iParameters, std::vector<float>& iMoments) const {
   float c0 = iParameters[0];
   float c1 = iParameters[1];
   //float c2 = iParameters[2];
//   float c3 = iParameters[3];

   float ensMean = Global::getMoment(iEnsemble.getValues(),1);
   float ensVar  = Global::getMoment(iEnsemble.getValues(),2);
   if(Global::isValid(ensMean) && Global::isValid(ensVar)) {
   //   float mean    = exp(c0) + exp(c1) * pow(ensMean, (float) 1/3);
      float mean = exp(c0) * pow(ensMean, (float) 1/2);
//float var     = exp(c2) + mean;
      //float var     = exp(c2) + exp(c3) * pow(ensMean, (float) 1/3);
      float var     = exp(c1) * pow(ensMean, (float) 1/3);
      assert(Global::isValid(var));

      iMoments.push_back(mean);
      iMoments.push_back(var);
      return true;
   }
   else {
      return false;
   }
}
