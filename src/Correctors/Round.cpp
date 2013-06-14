#include "Round.h"
#include "../Obs.h"
#include "../Parameters.h"

CorrectorRound::CorrectorRound(const Options& iOptions, const Data& iData) :
      Corrector(iOptions, iData),
      mRoundDownTo(Global::MV),
      mRoundUpTo(Global::MV) {
   iOptions.getValue("roundDownTo", mRoundDownTo);
   iOptions.getValue("roundUpTo", mRoundUpTo);
   if(!Global::isValid(mRoundDownTo) && !Global::isValid(mRoundUpTo)) {
      Global::logger->write("One or more of 'roundDownTo' and 'roundUpTo' must be specified for CorrectorRound", Logger::error);
   }
}
void CorrectorRound::correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const {
   float threshold = iParameters[0];
   for(int i = 0; i < iUnCorrected.size(); i++) {
      float fcst = iUnCorrected[i];
      if(Global::isValid(fcst) && fcst < threshold) {
         iUnCorrected[i] = mRoundDownTo;
      }
   }
}

void CorrectorRound::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> param;
   param.push_back(mRoundDownTo);
   
   iParameters.setAllParameters(param);
}

void CorrectorRound::updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   int counter = 0;
   float total = 0;
   for(int i = 0; i < iUnCorrected.size(); i++) {
      float fcst = iUnCorrected[i].getMoment(1);
      if(Global::isValid(fcst)) {
         // Find the average forecast value when the obs is on the lower boundary
         float obs = iObs[i].getValue();
         if(Global::isValid(obs)) {
            // TODO
            float min = mRoundDownTo; // Variable::get(iVariable)->getMin();
            if(obs == min) {
               total += fcst;
               counter++;
            }
         }
      }
   }

   // Set new parameters
   if(counter > 0) {
      iParameters[0] = Component::combine(iParameters[0], (float) total / counter, counter);
   }
}
