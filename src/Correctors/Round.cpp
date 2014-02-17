#include "Round.h"
#include "../Obs.h"
#include "../Parameters.h"

CorrectorRound::CorrectorRound(const Options& iOptions, const Data& iData) :
      Corrector(iOptions, iData),
      mRoundDownTo(Global::MV),
      mRoundUpTo(Global::MV),
      mFixed(Global::MV) {
   //! Round ensemble members down to this value
   iOptions.getValue("roundDownTo", mRoundDownTo);
   //! Round ensemble members up to this value
   iOptions.getValue("roundUpTo", mRoundUpTo);
   if(Global::isValid(mRoundDownTo) + Global::isValid(mRoundUpTo) != 1) {
      Global::logger->write("One of 'roundDownTo' and 'roundUpTo' must be specified for CorrectorRound", Logger::error);
   }
   //! Use a fixed threshold for determining when to apply rounding
   iOptions.getValue("fixed", mFixed);

   // TODO:
   // * Use the lower or upper boundary of the variable to determine what to round to
   
}
void CorrectorRound::correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const {
   float threshold;
   if(Global::isValid(mFixed))
      threshold = mFixed;
   else
      threshold = iParameters[0];

   for(int i = 0; i < iUnCorrected.size(); i++) {
      float fcst = iUnCorrected[i];
      // Round values down
      if(Global::isValid(mRoundDownTo)) {
         if(Global::isValid(fcst) && fcst < threshold) {
            iUnCorrected[i] = mRoundDownTo;
         }
      }
      // Round values up
      else {
         if(Global::isValid(fcst) && fcst > threshold) {
            iUnCorrected[i] = mRoundUpTo;
         }
      }
   }
}

void CorrectorRound::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> param;
   if(Global::isValid(mFixed))
      param.push_back(mFixed);
   else if(Global::isValid(mRoundDownTo))
      param.push_back(mRoundDownTo);
   else
      param.push_back(mRoundUpTo);
   iParameters.setAllParameters(param);
}

void CorrectorRound::updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   // Don't train if using a fixed threshold
   if(Global::isValid(mFixed))
      return;

   int counter = 0;
   float total = 0;
   for(int i = 0; i < iUnCorrected.size(); i++) {
      float fcst = iUnCorrected[i].getMoment(1);
      if(Global::isValid(fcst)) {
         // Find the average forecast value when the obs is on the lower boundary
         float obs = iObs[i].getValue();
         if(Global::isValid(obs)) {
            if(Global::isValid(mRoundDownTo) && obs == mRoundDownTo) {
               total += fcst;
               counter++;
            }
            else if(Global::isValid(mRoundUpTo) && obs == mRoundUpTo) {
               total += fcst;
               counter++;
            }
         }
      }
   }

   // Set new parameters
   if(counter > 0) {
      iParameters[0] = Processor::combine(iParameters[0], (float) total / counter, counter);
   }
}
