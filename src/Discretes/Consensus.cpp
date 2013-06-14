#include "Consensus.h"
#include "../Obs.h"
#include "../BaseDistributions/BaseDistribution.h"
#include "../Scheme.h"
#include "../Parameters.h"

DiscreteConsensus::DiscreteConsensus(const Options& iOptions, const Data& iData) : Discrete(iOptions, iData) {
}

float DiscreteConsensus::getPCore(const Ensemble& iEnsemble, const Parameters& iParameters) const {
   int count = 0;
   int N     = iEnsemble.size();
   int total = 0;
   for(int i = 0; i < N; i++) {
      if(Global::isValid(iEnsemble[i])) {
         if(mX == iEnsemble[i]) {
            count++;
         }
         total++;
      }
   }
   if(total == 0) {
      return Global::MV;
   }
   else {
      // The 0.5 comes from half of the mass falling on the boundary
      return (float) (0.5 + count)/(total+1);
   }
}
