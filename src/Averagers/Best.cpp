#include "Averager.h"
#include "Best.h"

AveragerBest::AveragerBest(const Options& iOptions, const Data& iData) : Averager(iOptions, iData) {
}

float AveragerBest::average(const Ensemble& iValues, const Parameters& iParameters) const {
   if(iValues.size() == iParameters.size()) {
      int index = Global::MV;
      float minError = Global::MV;

      // Find the member with lowest error
      for(int k = 0; k < iValues.size(); k++) {
         float value = iValues[k];
         float currError = iParameters[k];
         if(Global::isValid(value) && Global::isValid(currError)) {
            if(!Global::isValid(minError) || currError < minError) {
               minError = currError;
               index = k;
            }
         }

      }
      if(Global::isValid(index))
         return iValues[index];
      else
         // Default to ensemble mean
         return iValues.getMoment(1);
   }
   else {
      return iValues.getMoment(1);
   }
}
void AveragerBest::getDefaultParameters(Parameters& iParameters) const {
   iParameters[0] = Global::MV;
}
void AveragerBest::updateParameters(const std::vector<Ensemble>& iValues,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   std::vector<float> errors;
   std::vector<int> counter;
   for(int i = 0; i < iObs.size(); i++) {
      float obs = iObs[i].getValue();
      Ensemble ens = iValues[i];
      if(i == 0) {
         // Use the size of the first ensemble
         errors.resize(ens.size(), 0);
         counter.resize(ens.size(), 0);
      }
      // Compute the MAE for each ensemble member
      if(!Global::isMissing(obs) && ens.size() == errors.size()) {
         for(int k = 0; k < (int) ens.size(); k++) {
            if(!Global::isMissing(ens[k])) {
               float error = fabs(obs - ens[k]);
               errors[k] += error;
               counter[k]++;
            }
         }
      }
   }
   for(int k = 0; k < errors.size(); k++) {
      if(counter[k] > 0)
         errors[k] /= counter[k];
      else
         errors[k] = Global::MV;
   }

   // Update the parameters
   std::vector<float> params = iParameters.getAllParameters();
   if(iParameters.size() != errors.size()) {
      params.resize(errors.size(), 0);
   }
   for(int k = 0; k < errors.size(); k++) {
      if(Global::isValid(params[k]))
         params[k] = Processor::combine(params[k], errors[k], counter[k]);
      else
         params[k] = errors[k];
   }
   iParameters.setAllParameters(params);
}
