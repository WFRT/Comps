#include "Continuous.h"
#include "Bma.h"
#include "../Obs.h"

ContinuousBma::ContinuousBma(const Options& iOptions, const Data& iData) : Continuous(iOptions, iData) {
   // Assign the distribution used for each member
   std::string distributionTag;
   //! Tag of distribution to use
   iOptions.getRequiredValue("distribution", distributionTag);
   mBaseDistribution = BaseDistribution::getScheme(distributionTag, iData);
   iOptions.check();
}
ContinuousBma::~ContinuousBma() {
   delete mBaseDistribution;
}

float ContinuousBma::getCdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   std::vector<float> values = iEnsemble.getValues();
   int N = values.size();

   float variance = iParameters[0];
   float cdf = 0;
   float totalWeight = 0;
   std::vector<float> moments;
   moments.resize(2);
   for(int i = 0; i < N; i++) {
      float currValue = values[i];
      if(Global::isValid(currValue)) {
         moments[0] = currValue;
         moments[1] = variance;
         assert(Global::isValid(variance));
         assert(variance > 0);
         // Compute member weight
         float weight;
         if(iParameters.size()-1 == N)
            weight = iParameters[i+1];
         else
            weight = 1;

         cdf += weight * mBaseDistribution->getCdf(iX, moments);
         totalWeight += weight;
      }
   }
   if(totalWeight == 0)
      return Global::MV;
   else
      return cdf / totalWeight;
}
float ContinuousBma::getPdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const {
   std::vector<float> values = iEnsemble.getValues();
   int N = values.size();

   float variance = iParameters[0];
   float pdf = 0;
   float totalWeight = 0;
   std::vector<float> moments;
   moments.resize(2);
   for(int i = 0; i < N; i++) {
      float currValue = values[i];
      if(Global::isValid(currValue)) {
         moments[0] = currValue;
         moments[1] = variance;
         // Compute member weight
         float weight;
         if(iParameters.size()-1 == N)
            weight = iParameters[i+1];
         else
            weight = 1;

         pdf += weight * mBaseDistribution->getPdf(iX, moments);
         totalWeight += weight;
      }
   }
   if(totalWeight == 0)
      return Global::MV;
   else
      return pdf / totalWeight;
}

void ContinuousBma::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> param(2);
   param[0] = 1; // Variance
   param[1] = 1; // Weight

   iParameters.setAllParameters(param);
}

void ContinuousBma::updateParametersCore(const std::vector<Ensemble>& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const {
   // TODO: Check that this actually works since we have a vector of obs and ens now
   for(int i = 0; i < iObs.size(); i++) {
      float obs = iObs[i].getValue();
      if(!Global::isValid(obs))
         return;

      Ensemble ens = iEnsemble[i];

      std::vector<float> values = ens.getValues();
      int N = values.size();

      // Adjust weights
      float lastVariance = iParameters[0];
      if(iParameters.size()-1 == N) {

         float totalZ = 0;
         std::vector<float> z(N);
         std::vector<float> moments(2);
         moments[1] = lastVariance; // 2nd moment
         // Loop over all weights
         // Set the new weight based on the pdf at the obs
         for(int i = 0; i< N; i++) {
            if (Global::isValid(values[i])) {
               moments[0] = values[i]; // 1st moment
               z[i] = iParameters[i+1] * mBaseDistribution->getPdf(obs, moments);
               assert(Global::isValid(z[i]));
               totalZ += z[i];
            }
            else {
               z[i] = Global::MV;
            }
         }
         if(totalZ > 0) {
            float s_sq = 0;
            for (int i = 0; i< N; i++) {
               if (z[i] != Global::MV) {
                  float z_star = z[i]/totalZ;
                  iParameters[i+1] = combine(iParameters[i+1],z_star);
                  s_sq += z_star*(pow(obs - values[i],2));
               }
            }
            assert(s_sq > 0);
            iParameters[0] = combine(lastVariance,s_sq);
            // check that weights sum to 1
            // it is possible that the sum could drift away from 1 when there are missing members
            // if members are all valid though, this shouldn't happen, because we are pushing the weights toward
            //   z_star, and these values are normalized by dividing by totalPdf
            float sumWeights = 0;
            for (int i = 0; i< N; i++) {
               sumWeights += iParameters[i+1];
            }
            for (int i = 0; i< N; i++) {
               iParameters[i+1] = iParameters[i+1]/sumWeights;
            }
         }
      }
      else {
         // The case when the parameter size doesn't match the ensemble size
         // This happens when you do this the first time
         std::vector<float> paramValues(N+1);
         // set initial weights to be equal
         for (int i = 1; i< N+1; i++) {
            paramValues[i] = (float) 1/N;
         }
         // set initial variance to variance of ensemble members?
         float ensMean = Global::getMoment(values,2);
         if(Global::isValid(ensMean))
            paramValues[0] = ensMean;
         else
            paramValues[0] = 1;
         iParameters.setAllParameters(paramValues);
      }
   }
}
