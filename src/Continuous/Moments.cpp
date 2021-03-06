#include "Moments.h"
#include "../Obs.h"
#include "../BaseDistributions/BaseDistribution.h"
#include "../Scheme.h"
#include "../Parameters.h"
#include "../Measures/Measure.h"

ContinuousMoments::ContinuousMoments(const Options& iOptions, const Data& iData) :
      Continuous(iOptions, iData), mTransform(NULL) {
   std::string transformTag;
   if(iOptions.getValue("transform", transformTag)) {
      mTransform = Transform::getScheme(transformTag);
   }

   //! Regression type (one of 'full', 'ens', and 'const')
   iOptions.getRequiredValue("type", mType);
   if(mType != "full" && mType != "ens" && mType != "rawens" && mType != "const") {
      std::stringstream ss;
      ss << "ContinuousMoments: 'type' must be 'full', 'ens', 'rawens', or 'const'";
      Global::logger->write(ss.str(), Logger::error);
   }

   std::string distributionTag;
   //! Tag of distribution to use
   iOptions.getRequiredValue("distribution", distributionTag);
   mBaseDistribution = BaseDistribution::getScheme(distributionTag, iData);

   // UncertaintyMeasure
   std::string measureTag;
   //! Tag of measure to use in regression
   iOptions.getRequiredValue("measure", measureTag);
   mMeasure = Measure::getScheme(measureTag, iData);
   iOptions.check();
}
ContinuousMoments::~ContinuousMoments() {
   delete mBaseDistribution;
   delete mMeasure;
   if(mTransform)
      delete mTransform;
}

float ContinuousMoments::getCdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   std::vector<float> moments;
   if(getMoments(iEnsemble, iParameters, moments)) {
      //Transform iX if necessary
      if(mTransform != NULL) {
         iX = mTransform->transform(iX);
      }

      return mBaseDistribution->getCdf(iX, moments);
   }
   else {
      return Global::MV;
   }
}
float ContinuousMoments::getPdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   std::vector<float> moments;
   if(getMoments(iEnsemble, iParameters, moments)) {
      float iXorig = iX;
      //Transform iX if necessary
      if(mTransform != NULL) {
         iX = mTransform->transform(iX);
      }

      float pdfVal =  mBaseDistribution->getPdf(iX, moments);
      if(mTransform != NULL) {
         pdfVal = mTransform->derivative(iXorig)*pdfVal;
      }
      return pdfVal;
   }
   else {
      return Global::MV;
   }
}
float ContinuousMoments::getMomentCore(int iMoment, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   if(iMoment <= 2) {
      std::vector<float> moments;
      getMoments(iEnsemble, iParameters, moments);
      return moments[iMoment - 1];
   }
   else {
      assert(0);
      return 1; //Continuous::getMoment(iMoment, iEnsemble, mData, iParameters);
   }
}
bool ContinuousMoments::getMoments(const Ensemble& iEnsemble, const Parameters& iParameters, std::vector<float>& iMoments) const {
   iMoments.clear();

   // Transform iEnsemble.getValues()
   float mean;
   float measure;
   if(mTransform != NULL) {
      Ensemble ens = iEnsemble;
      mTransform->transform(ens);
      mean = ens.getMoment(1);
      measure  = mMeasure->measure(ens);
   }
   else {
      mean = iEnsemble.getMoment(1);
      measure = mMeasure->measure(iEnsemble);
   }

   iMoments.resize(2,Global::MV);
   if(!Global::isValid(mean) || !Global::isValid(measure)) {
      // TODO
      return false;
   }

   float a0;
   float a1;
   if(mType == "rawens") {
      a1 = 1;
      a0 = 0;
      if(measure == 0) {
         Global::logger->write("ContinousMoments: variance of distribution is 0 because the 'rawens' type is used when the uncertainty measure is also 0", Logger::error);
      }
   }
   else {
      std::vector<float> param;
      param = iParameters.getAllParameters();
      assert(param.size() == 4);
      float error2        = param[0];
      float variance      = param[1];
      float variance2     = param[2];
      float error2Variance = param[3];
      if(mType == "full") {
         a1 = (error2Variance - variance*error2)/(variance2 - variance*variance);
         a0 = error2 - a1*variance;
         if(a1 < 0 || a0 < 0) {
            a1 = 0;
            a0 = error2;
         }
         else if(a1 == 0 && a0 == 0) {
            a1 = 0;
            a0 = error2;
         }
      }
      else if(mType == "ens") {
         a1 = (error2/variance);
         a0 = 0;
      }
      else {
         a1 = 0;
         a0 = error2;
      }

      // Sanity check
      if(a1 < 0 || std::isnan(a1) || std::isinf(a1))
         a1 = 0;
      if(a0 < 0 || std::isnan(a0) || std::isinf(a0))
         a0 = 0;
      if(a0 == 0 && measure == 0) {
         //Global::logger->write("ContinuousMoments: measure is 0, reverting to constant spread", Logger::warning);
         a0 = error2;
         a1 = 0;
      }
      // Revert to constant variance when variance is 0
      if(a0 == 0 && a1 == 0) {
         a0 = error2;
      }
   }

   float moment0 = mean;
   float moment1 = a0 + a1 * measure;
   assert(!std::isnan(moment1));
   assert(!std::isinf(moment1));
      
   iMoments[0] = moment0;
   iMoments[1] = moment1;
   assert(moment1 > 0);

   return true;
}

void  ContinuousMoments::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> param(4,Global::MV);
   param[0] = 1; // Error ^ 2
   param[1] = 1; // Variance
   param[2] = 2; // Variance ^ 2
   param[3] = 1; // Error * Variance
   iParameters.setAllParameters(param);
}

void ContinuousMoments::updateParametersCore(const std::vector<Ensemble>& iEnsemble,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   assert(iParameters.size() == 4);
   float error2         = 0;
   float variance       = 0;
   float variance2      = 0;
   float error2Variance = 0;
   int N = 0;

   for(int i = 0; i < iObs.size(); i++) {
      float obs = iObs[i].getValue();
      Ensemble ens = iEnsemble[i];

      float currMean;
      float currMeasure;
      if(mTransform != NULL) {
         mTransform->transform(ens);
         obs           = mTransform->transform(obs);
         currMean      = ens.getMoment(1);
         currMeasure   = mMeasure->measure(ens);
      }
      else {
         currMean      = ens.getMoment(1);
         currMeasure   = mMeasure->measure(ens);
      }

      if(Global::isValid(obs) && Global::isValid(currMean) && Global::isValid(currMeasure)) {
         float currError = obs - currMean;
         error2         += currError*currError;
         variance       += currMeasure;
         variance2      += currMeasure*currMeasure;
         error2Variance += currError*currError*currMeasure;
         N++;
      }
   }
   if(N > 0) {
      iParameters[0] = combine(iParameters[0], error2/N, N);
      iParameters[1] = combine(iParameters[1], variance/N, N);
      iParameters[2] = combine(iParameters[2], variance2/N, N);
      iParameters[3] = combine(iParameters[3], error2Variance/N, N);
   }
}

bool ContinuousMoments::needsTraining() const {
   return mType != "rawens";
}
