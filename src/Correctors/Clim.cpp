#include "Clim.h"
#include "../Obs.h"
#include "../Parameters.h"
#include "../Data.h"

CorrectorClim::CorrectorClim(const Options& iOptions, const Data& iData) : Corrector(iOptions, iData) {}
void CorrectorClim::correctCore(const Parameters& iParameters, Ensemble& iEnsemble) const {
   float climWeight = iParameters[0];
   float ensWeight = 1 - climWeight;
   float clim = mData.getClim(iEnsemble.getDate(), iEnsemble.getInit(), iEnsemble.getOffset(),
                              iEnsemble.getLocation(), iEnsemble.getVariable());
   if(Global::isValid(clim) && Global::isValid(ensWeight) && Global::isValid(climWeight)) {
      for(int n = 0; n < iEnsemble.size(); n++) {
         float currValue = iEnsemble[n];
         if(Global::isValid(currValue)) {
            iEnsemble[n] = currValue * ensWeight + clim * climWeight;
         }
      }
   }
}

void CorrectorClim::updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   int isClimBetter = 0;
   int counter = 0;
   for(int i = 0; i < iObs.size(); i++) {
      float obs = iObs[0].getValue();
      float clim = mData.getClim(iUnCorrected[i].getDate(), iUnCorrected[i].getInit(),
                                 iUnCorrected[i].getOffset(), iUnCorrected[i].getLocation(),
                                 iUnCorrected[i].getVariable());
      float ensMean = iUnCorrected[i].getMoment(1);
      if(Global::isValid(obs) && Global::isValid(clim) && Global::isValid(ensMean)) {
         isClimBetter += fabs(obs - clim) < fabs(obs - ensMean);
         counter++;
      }
   }
   if(counter > 0) {
      iParameters[0] = Component::combine(iParameters[0], (float) isClimBetter/counter, counter);
   }
}
void CorrectorClim::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> param;
   param.push_back(0); // Climatological weight
   iParameters.setAllParameters(param);
}
