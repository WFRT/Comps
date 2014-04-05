#include "MultiVariateRegression.h"
#include "../Data.h"
#include "../Obs.h"
#include "../Parameters.h"

CorrectorMultiVariateRegression::CorrectorMultiVariateRegression(const Options& iOptions, const Data& iData) :
      Corrector(iOptions, iData) {
   Component::underDevelopment();
   //! List of variables used in regression
   iOptions.getRequiredValues("variables", mVariables);
   assert(mVariables.size() > 0);
}
void CorrectorMultiVariateRegression::correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const {

   int N = iUnCorrected.size();
   std::vector<float> correctedValues(N, 0);
   int V = (int) mVariables.size();
   float correctedValue = 0;
   for(int i = 0; i < (int) mVariables.size(); i++) {
      Ensemble ens = mData.getEnsemble(iUnCorrected.getDate(), iUnCorrected.getInit(), iUnCorrected.getOffset(),
            iUnCorrected.getLocation(), mVariables[i]);
      if(ens.size() != N) {
         std::stringstream ss;
         ss << "CorrectorMultiVariateRegression: The ensemble of all variables must be the same. Expected size: "
            << N << " size for " << mVariables[i] << " " << ens.size();
         Global::logger->write(ss.str(), Logger::warning);
         return;
      }
      for(int n = 0; n < N; n++) {
         float value = ens[n];
         // TODO:
         if(!Global::isValid(value)) {
            std::stringstream ss;
            ss << "CorrectorMultiVariateRegression: Missing variable: mVariables[i]";
            Global::logger->write(ss.str(), Logger::warning);
            return;
         }
         assert(iParameters[i] != 0);
         correctedValues[n] += iParameters[V+i] * value / iParameters[i];
      }
   }
   assert(!std::isnan(correctedValue) && !std::isinf(correctedValue));
   for(int n = 0; n < N; n++) {
      // Add on constant term
      iUnCorrected[n] = correctedValues[n] + iParameters[2*V];
   }
}

void CorrectorMultiVariateRegression::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> coeffs;
   for(int i = 0; i < (int) mVariables.size(); i++) {
      coeffs.push_back(1); // Mean XX
      coeffs.push_back(1); // Mean XY
   }
   // Mean obs
   coeffs.push_back(1);
   iParameters.setAllParameters(coeffs);
}

void CorrectorMultiVariateRegression::updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   int V = (int) mVariables.size();
   std::vector<float> currMeanXX;
   std::vector<float> currMeanXY;
   float currMeanObs = 0;
   std::vector<int> counters;
   int counterObs = 0;
   for(int v = 0; v < V; v++) {
      currMeanXX.push_back(0);
      currMeanXY.push_back(0);
      counters.push_back(0);
   }
   /*
   for(int i = 0; i < (int) iObs.size(); i++) {
      float obs = iObs[i].getValue();
      if(Global::isValid(obs)) {
         for(int v = 0; v < V; v++) {
            std::vector<float> values;
            mData.getInput()->getValues(iDate, iInit, iOffset, iLocation.getId(), mVariables[i], values);
            float value = Global::mean(values);
            
            if(Global::isValid(value)) {
               currMeanXX[v] += value * value;
               counters[v]++;
               currMeanXY[v] += value * obs;
            }
         }
         currMeanObs += obs;
         counterObs++;
      }
   }
  */
   for(int v = 0; v < V; v++) {
      if(counters[v] != 0) {
         // Only update parameters if we have valid observations and forecasts
         iParameters[v]   = combine(iParameters[v],   currMeanXX[v]/counters[v]);
         iParameters[V+v] = combine(iParameters[V+v], currMeanXY[v]/counters[v]);
         //std::cout << "iParameters[" << v << "] = " << iParameters[v] << std::endl;
         //std::cout << "iParameters[" << V+v << "] = " << iParameters[V+v] << std::endl;
      }
   }
   if(counterObs != 0) {
      iParameters[2*V] = combine(iParameters[2*V], currMeanObs/counterObs);
   }
}
int CorrectorMultiVariateRegression::numParametersCore() const {
   return 2*mVariables.size() + 1;
}
