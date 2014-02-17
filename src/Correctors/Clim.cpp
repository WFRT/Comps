#include "Clim.h"
#include "../Obs.h"
#include "../Parameters.h"
#include "../Data.h"

CorrectorClim::CorrectorClim(const Options& iOptions, const Data& iData) : Corrector(iOptions, iData),
      mComputeClim(0) {
      
   //! Should method compute the climatology (alternatively, look it up)?
   iOptions.getValue("computeClim", mComputeClim);
}
void CorrectorClim::correctCore(const Parameters& iParameters, Ensemble& iEnsemble) const {
   float climWeight = iParameters[0];
   float ensWeight = 1 - climWeight;
   float clim;
   if(mComputeClim) {
      assert(iParameters.size() == 2);
      clim = iParameters[1];
   }
   else
      clim = mData.getClim(iEnsemble.getDate(), iEnsemble.getInit(), iEnsemble.getOffset(),
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
   float totalObs = 0;
   int counterObs = 0;
   for(int i = 0; i < iObs.size(); i++) {
      float obs = iObs[0].getValue();
      float clim;
      if(mComputeClim) {
         assert(iParameters.size() == 2);
         clim = iParameters[1];
      }
      else {
         assert(iParameters.size() == 1);
         clim = mData.getClim(iUnCorrected[i].getDate(), iUnCorrected[i].getInit(),
               iUnCorrected[i].getOffset(), iUnCorrected[i].getLocation(),
               iUnCorrected[i].getVariable());
      }
      float ensMean = iUnCorrected[i].getMoment(1);
      if(Global::isValid(obs) && Global::isValid(clim) && Global::isValid(ensMean)) {
         isClimBetter += fabs(obs - clim) < fabs(obs - ensMean);
         counter++;
      }
      if(Global::isValid(obs)) {
         totalObs += obs;
         counterObs++;
      }
   }
   if(counter > 0) {
      iParameters[0] = Processor::combine(iParameters[0], (float) isClimBetter/counter, counter);
   }
   if(mComputeClim && counterObs > 0) {
      assert(iParameters.size() == 2);
      if(Global::isValid(iParameters[1]))
         iParameters[1] = Processor::combine(iParameters[1], (float) totalObs/counterObs, counter);
      else
         iParameters[1] = (float) totalObs/counterObs;
   }
}
void CorrectorClim::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> param;
   param.push_back(0); // Climatological weight
   if(mComputeClim)
      param.push_back(Global::MV);
   iParameters.setAllParameters(param);
}
