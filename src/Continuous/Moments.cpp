#include "Moments.h"
#include "../Obs.h"
#include "../BaseDistributions/BaseDistribution.h"
#include "../Scheme.h"
#include "../Parameters.h"
#include "../Measures/Measure.h"

ContinuousMoments::ContinuousMoments(const Options& iOptions, const Data& iData) :
      Continuous(iOptions, iData), mDoLogTransform(false) {
   iOptions.getRequiredValue("type", mType);
   if(mType != "full" && mType != "ens" && mType != "rawens" && mType != "const") {
      std::stringstream ss;
      ss << "ContinuousMoments: 'type' must be 'full', 'ens', 'rawens', or 'const'";
      Global::logger->write(ss.str(), Logger::error);
   }

   std::string distributionTag;
   iOptions.getRequiredValue("distribution", distributionTag);
   mBaseDistribution = BaseDistribution::getScheme(distributionTag, iData);

   iOptions.getValue("logTransform", mDoLogTransform);

   // UncertaintyMeasure
   std::string measureTag;
   iOptions.getRequiredValue("measure", measureTag);
   mMeasure = Measure::getScheme(measureTag, iData);
   if(!mMeasure->isPositiveDefinite()) {
      std::stringstream ss;
      ss << "ContinousMoments should use a measure that is positive definite, otherwise the distributions variance may be negative or 0";
      Global::logger->write(ss.str(), Logger::critical);
   }
}
ContinuousMoments::~ContinuousMoments() {
   delete mBaseDistribution;
   delete mMeasure;
}

float ContinuousMoments::getCdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   std::vector<float> moments;
   if(getMoments(iEnsemble, iParameters, moments)) {
      //Transform iX if necessary
      if(mDoLogTransform)
      {
         iX = log(iX);
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
      if(mDoLogTransform)
      {
         iX = log(iX);
      }
      float pdfVal =  mBaseDistribution->getPdf(iX, moments);
      // Note probably have to multiply by some kind of derivative
      if(mDoLogTransform)
      {
         pdfVal = (1/iXorig)*pdfVal;
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
   int numMoments = mBaseDistribution->getNumMoments();
   iMoments.clear();

   // Transform iEnsemble.getValues()
   float mean;
   float measure;
   // Only do if mDoLogTransform
   if(mDoLogTransform)
   {
      std::vector<float> values = iEnsemble.getValues();
      // log transform values
      for(int i = 0; i < values.size(); i++) 
      {
         if (Global::isValid(values[i]))
         {
            values[i] = log(values[i]);
         }
      }
      mean = Global::getMoment(values,1);

      Ensemble ens = iEnsemble;
      ens.setValues(values);
      measure  = mMeasure->measure(iEnsemble,Parameters());
   }
   else {
      mean = iEnsemble.getMoment(1);
      measure = mMeasure->measure(iEnsemble,Parameters());
   }

   float moment0;
   float moment1;
   if(!Global::isValid(mean) || !Global::isValid(measure)) {
      moment0 = Global::MV;
      moment1 = Global::MV;
      iMoments.push_back(moment0);
      iMoments.push_back(moment1);
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

   moment0 = mean;
   moment1 = a0 + a1 * measure;
   assert(!std::isnan(moment1));
   assert(!std::isinf(moment1));
      
   iMoments.push_back(moment0);
   iMoments.push_back(moment1);
   assert(moment1 > 0);

   return true;
}

void  ContinuousMoments::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> param;
   param.push_back(1); // Error ^ 2
   param.push_back(1); // Variance
   param.push_back(2); // Variance ^ 2
   param.push_back(1); // Error * Variance
   iParameters.setAllParameters(param);
}

void ContinuousMoments::updateParametersCore(
      const Ensemble& iEnsemble,
      const Obs& iObs,
      Parameters& iParameters) const {
   std::vector<float> param;
   param = iParameters.getAllParameters();
   assert(param.size() == 4);

   float obs = iObs.getValue();
   float error2        = param[0];
   float variance      = param[1];
   float variance2     = param[2];
   float error2Variance = param[3];

   float currMean;
   float currMeasure;
   // The ensemble values should be transformed
   if(mDoLogTransform)
   {
      std::vector<float> values = iEnsemble.getValues();
      if (Global::isValid(obs))
      {
         obs = log(obs);
      }
      // log transform values
      for(int i = 0; i < values.size(); i++) 
      {
         if (Global::isValid(values[i]))
         {
            values[i] = log(values[i]);
         }
      }
      currMean      = Global::getMoment(values,1);

      Ensemble ens = iEnsemble;
      ens.setValues(values);
      currMeasure   = mMeasure->measure(ens,Parameters());
   }
   else {
      currMean      = iEnsemble.getMoment(1);
      currMeasure   = mMeasure->measure(iEnsemble,Parameters());
   }

   float currError;
   if(Global::isValid(obs) && Global::isValid(currMean) && Global::isValid(currMeasure)) {
      currError = obs - currMean;
      error2        = combine(error2, currError*currError);
      variance      = combine(variance, currMeasure);
      variance2     = combine(variance2, currMeasure*currMeasure);
      error2Variance = combine(error2Variance, currError*currError*currMeasure);

      std::vector<float> newValues;
      newValues.push_back(error2);
      newValues.push_back(variance);
      newValues.push_back(variance2);
      newValues.push_back(error2Variance);
      iParameters.setAllParameters(newValues);
      float a1 = (error2Variance - variance*error2)/(variance2 - variance*variance);
      float a0 = error2 - a1*variance;
      //std::cout << "Parameters: " << error2 << " " << variance << " " << variance2 << " " << error2Variance << " (" << a0 << ", " << a1 << ")" << std::endl;
   }
}

bool ContinuousMoments::needsTraining() const {
   return mType != "rawens";
}
