#include "Const.h"
#include "../Obs.h"
#include "../Scheme.h"
#include "../Parameters.h"
#include "../Variables/Variable.h"

DiscreteConst::DiscreteConst(const Options& iOptions, const Data& iData) :
      Discrete(iOptions, iData),
      mInitialP(0.1) {
   //! Starting value of the probability before training
   iOptions.getValue("initialP", mInitialP);
}

float DiscreteConst::getPCore(const Ensemble& iEnsemble, const Parameters& iParameters) const {
   float P = iParameters[0];
   return P;
}

void DiscreteConst::updateParametersCore(const std::vector<Ensemble>& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const {
   float P = iParameters[0];
   assert(P > 0 && P < 1);
   float accumP = 0;
   int counter = 0;
   for(int t = 0; t < (int) iObs.size(); t++) {
      float obsValue = iObs[t].getValue();
      if(Global::isValid(obsValue)) {
         if(obsValue == mX) {
            accumP += 1;
         }
         counter++;
      }
   }
   if(counter > 0) {
      //std::cout << "Old parameter = " << iParameters[0];
      iParameters[0] = Processor::combine(P, accumP/counter);
      //std::cout << " new = " << iParameters[0] << std::endl;
      assert(iParameters[0] > 0 && iParameters[0] < 1);
   }
   else {
      //std::cout << "Can't update" << std::endl;
   }
}
void DiscreteConst::getDefaultParameters(Parameters& iParameters) const {
   std::vector<float> param;
   param.push_back(mInitialP); // P
   iParameters.setAllParameters(param);
}
