#include "Directional.h"
#include "../Data.h"
#include "../Obs.h"
#include "../Parameters.h"
#include "../Ensemble.h"
CorrectorDirectional::CorrectorDirectional(const Options& iOptions, const Data& iData) :
      Corrector(iOptions, iData),
      mMultiplicative(false),
      mVariable("WindDir") {
   //! How many bins should the direction be divided into?
   iOptions.getRequiredValue("numPoints", mNumPoints);
   //! Should the correction be multiplicative instead of additive?
   iOptions.getValue("multiplicative", mMultiplicative);

   // How many frequencies should be used to capture correction?
   // Fourier filter
   // iOptions.getRequiredValue("numWaves", mNumWaves);
}
void CorrectorDirectional::correctCore(const Parameters& iParameters,
      Ensemble& iUnCorrected) const {
   float direction  = getDirection(iUnCorrected);
   float correction = computeCorrection(direction, iParameters);
   if(!Global::isValid(correction))
      return;

   // Correct members
   for(int i = 0; i < iUnCorrected.size(); i++) {
      if(Global::isValid(iUnCorrected[i])) {
         if(mMultiplicative) {
            iUnCorrected[i] *= correction;
         }
         else {
            iUnCorrected[i] += correction;
         }
      }
   }
}

void CorrectorDirectional::getDefaultParametersCore(Parameters& iParameters) const {
   // For additive correction, all bins are initialized to 0
   // For multiplicative correction, all bins are initialized to 1
   float initValue = mMultiplicative ? 1 : 0;
   std::vector<float> values(mNumPoints*2, initValue);
   iParameters.setAllParameters(values);
}

void CorrectorDirectional::updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {

   // Add up obs and forecasts into wind direction bins
   std::vector<float> accum(mNumPoints*2,0);
   int counter = 0;
   for(int i = 0; i < iObs.size(); i++) {
      float obs  = iObs[i].getValue();
      float fcst = iUnCorrected[i].getMoment(1);

      // Get wind direction
      float direction = getDirection(iUnCorrected[i]);
      if(Global::isValid(obs) && Global::isValid(fcst) && Global::isValid(direction)) {
         int index = getBinIndex(direction);
         accum[2*index] += obs;
         accum[2*index+1] += fcst;
         counter++;
      }
   }

   // Update parameters
   for(int i = 0; i < accum.size(); i++) {
      iParameters[i] = Processor::combine(iParameters[i], accum[i], counter);
   }
}

float CorrectorDirectional::computeCorrection(float iDirection, const Parameters& iParameters) const {
   if(!Global::isValid(iDirection))
      return Global::MV;

   assert(iParameters.size() == mNumPoints*2);
   int index = getBinIndex(iDirection);
   float meanObs  = iParameters[index*2];
   float meanFcst = iParameters[index*2+1];
   float correction;
   if(mMultiplicative)
      correction = meanObs / meanFcst;
   else
      correction = meanObs - meanFcst;

   /*
    Fourier filter to get frequencies
   float correction = iParameters[0];
   float dirRad = iDirection  * Global::pi / 180;
   for(int counter = 0; counter < mNumWaves; counter++) {
      float freq = (counter+1);
      float ampSin = iParameters[counter*2+1];
      float ampCos = iParameters[counter*2+2];
      correction += ampSin * sin(dirRad * freq) + ampCos * cos(dirRad * freq);
   }
   */

   return correction;
}

float CorrectorDirectional::getDirection(const Ensemble& iEnsemble) const {
   Ensemble ens;
   mData.getEnsemble(iEnsemble.getDate(), iEnsemble.getInit(), iEnsemble.getOffset(),
         iEnsemble.getLocation(), mVariable, Input::typeForecast, ens);
   return ens.getMoment(1);
}

int CorrectorDirectional::getBinIndex(int iDirection) const {
   assert(Global::isValid(iDirection));
   assert(iDirection >= 0 && iDirection < 360);
   int index = floor(iDirection / 360.0 * mNumPoints);
   assert(index >= 0 && index < mNumPoints);
   return index;
}
