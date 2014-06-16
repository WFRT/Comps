#include "Best.h"
#include "../Obs.h"
#include "../Parameters.h"

CorrectorBest::CorrectorBest(const Options& iOptions, const Data& iData) : Corrector(iOptions, iData), mNum(1) {
   //! How many of the best members should be used?
   iOptions.getValue("num", mNum);
}
void CorrectorBest::correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const {
   // Default to missing
   std::vector<float> values(1,Global::MV);

   if(iParameters.size() == iUnCorrected.size()) {
      int N = iParameters.size();

      // Sort ensemble based on skill
      std::vector<std::pair<float,float> > pairs; // fcst, score
      for(int n = 0; n < N; n++) {
         if(Global::isValid(iParameters[n]))
            pairs.push_back(std::pair<float,float>(iUnCorrected[n], iParameters[n]));
      }
      std::sort(pairs.begin(), pairs.end(), Global::sort_pair_second<int, float>());

      // Create new ensemble
      if(pairs.size() > 0) {
         int num = std::min(mNum, (int) pairs.size());
         values.resize(num);

         for(int i = 0; i < num; i++) {
            values[i] = pairs[i].first;
         }
      }
   }
   iUnCorrected.setValues(values);
}

void CorrectorBest::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> param(1);
   param[0] = Global::MV;
   iParameters.setAllParameters(param);
}

void CorrectorBest::updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   if(iUnCorrected.size() == 0)
      return;

   // Resize parameters if wrong size
   int N = iUnCorrected[0].size();
   if(iParameters.size() != N) {
      std::vector<float> values(N, Global::MV);
      iParameters.setAllParameters(values);
   }

   // Compute today's errors
   std::vector<float> todaysError(N, 0);
   std::vector<int> counter(N, 0);
   for(int t = 0; t < iObs.size(); t++) {
      Ensemble ens = iUnCorrected[t];
      if(ens.size() != N) {
         std::stringstream ss;
         ss << "CorrectorBest: Ensembles do not have constant size (sizes: " << N << " and " << ens.size() << ") ";
         ss << "Cannot update parameters.";
         Global::logger->write(ss.str(), Logger::warning);
         return;
      }
      float obs = iObs[t].getValue();
      if(Global::isValid(obs)) {
         for(int i = 0; i < N; i++) {
            float fcst = ens[i];
            if(Global::isValid(fcst)) {
               todaysError[i] += fabs(fcst - obs);
               counter[i]++;
            }
         }
      }
   }

   // Update
   for(int i = 0; i < N; i++) {
      if(counter[i] > 0) {
         if(Global::isValid(iParameters[i])) {
            iParameters[i] = combine(iParameters[i], todaysError[i]/counter[i]);
         }
         else {
            iParameters[i] = todaysError[i]/counter[i];
         }
      }
   }
}
