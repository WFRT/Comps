#include "ClimReg.h"
#include "../Obs.h"
#include "../Parameters.h"
#include "../Data.h"

CorrectorClimReg::CorrectorClimReg(const Options& iOptions, const Data& iData) :
   Corrector(iOptions, iData) {
   iOptions.check();
}
void CorrectorClimReg::correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const {
   float meanAnom2 = iParameters[2];
   float meanErrorAnom = iParameters[3];
   float clim = mData.getClim(iUnCorrected.getDate(), iUnCorrected.getInit(), iUnCorrected.getOffset(),
                              iUnCorrected.getLocation(), iUnCorrected.getVariable());

   if(Global::isValid(clim)) {
      for(int n = 0; n < iUnCorrected.size(); n++) {
         float fcst = iUnCorrected[n];
         if(Global::isValid(fcst)) {
            float a = meanErrorAnom / meanAnom2;
            iUnCorrected[n] = a * clim + (1-a) * fcst;
         }
      }
   }
}

void CorrectorClimReg::updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   float oldMeanError = iParameters[0];
   float oldMeanAnom = iParameters[1];
   float oldMeanAnom2 = iParameters[2];
   float oldMeanErrorAnom = iParameters[3];
   float currMeanError = 0;
   float currMeanAnom = 0;
   float currMeanAnom2 = 0;
   float currMeanErrorAnom = 0;
   int counter = 0;
   for(int i = 0; i < iObs.size(); i++) {
      float obs = iObs[i].getValue();
      float clim = mData.getClim(iUnCorrected[i].getDate(), iUnCorrected[i].getInit(),
                     iUnCorrected[i].getOffset(), iUnCorrected[i].getLocation(),
                     iUnCorrected[i].getVariable());

      float fcst = iUnCorrected[i].getMoment(1);
      if(Global::isValid(obs) && Global::isValid(clim) && Global::isValid(fcst)) {
         currMeanError += obs - fcst;
         currMeanAnom  += clim - fcst;
         currMeanAnom2 += (clim - fcst)*(clim - fcst);
         currMeanErrorAnom += (obs - fcst)*(clim - fcst);
         counter++;
      }
   }
   if(counter > 0) {
      iParameters[0] = combine(oldMeanError, currMeanError/counter, counter);
      iParameters[1] = combine(oldMeanAnom, currMeanAnom/counter, counter);
      iParameters[2] = combine(oldMeanAnom2, currMeanAnom2/counter, counter);
      iParameters[3] = combine(oldMeanErrorAnom, currMeanErrorAnom/counter, counter);
   }
}
void CorrectorClimReg::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> coeffs;
   coeffs.push_back(0); // meanError
   coeffs.push_back(0); // meanAnom
   coeffs.push_back(1); // meanAnom2
   coeffs.push_back(0); // meanErrorAnom
   iParameters.setAllParameters(coeffs);
}

// y = a * clim + (1-a) * fcst
// y - fcst = a * (clim - fcst)
// y = a + b * fcst
// Y = A * [1 F]
