#include "Conditional.h"
#include "../Obs.h"
#include "../Parameters.h"

CorrectorConditional::CorrectorConditional(const Options& iOptions, const Data& iData) :
      Corrector(iOptions, iData) {
   iOptions.getRequiredValues("edges", mEdges);

   std::string interpolatorTag;
   iOptions.getRequiredValue("interpolator", interpolatorTag);
   mInterpolator = Interpolator::getScheme(interpolatorTag);

   for(int i = 1; i < mEdges.size(); i++) {
      mCenters.push_back((mEdges[i]+mEdges[i-1])/2);
   }

   iOptions.check();
}

void CorrectorConditional::correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const {
   // Correct each ensemble member
   for(int n = 0; n < iUnCorrected.size(); n++) {
      float fcst = iUnCorrected[n];
      if(Global::isValid(fcst) && fcst >=  mEdges[0] && fcst <= mEdges[mEdges.size()-1]) {
         int lowerIndex = Global::MV;
         int upperIndex = Global::MV;
         // Find which bin the forecast is in
         for(int i = 1; i < mEdges.size(); i++) {
            if(fcst <= mEdges[i]) {
               lowerIndex = i-1;
               upperIndex = i;
               break;
            }
         }

         int B = iParameters.size();
         assert(mCenters.size() == B);
         if(fcst < mCenters[0]) {
            iUnCorrected[n] = iParameters[0];
         }
         else if(fcst > mCenters[B-1]) {
            iUnCorrected[n] = iParameters[B-1];
         }
         else {
            for(int i = 1; i < mCenters.size(); i++) {
               if(fcst < mCenters[i]) {
                  // Interpolate
                  float lowerFcst = mCenters[i-1];
                  float upperFcst = mCenters[i];
                  float lowerObs  = iParameters[i-1];
                  float upperObs  = iParameters[i];
                  if(lowerFcst == upperFcst) {
                     iUnCorrected[n] = (lowerObs+upperObs)/2;
                  }
                  else {
                     iUnCorrected[n] = lowerObs + (fcst - lowerFcst) * (upperObs-lowerObs)/(upperFcst - lowerFcst);
                  }
                  break;
               }
            }
         }
      }
   }
}

void CorrectorConditional::getDefaultParametersCore(Parameters& iParameters) const {
   iParameters.setAllParameters(mCenters);
}

void CorrectorConditional::updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   std::vector<float> param = iParameters.getAllParameters();

   std::vector<float> totalObs(iParameters.size(), 0);
   std::vector<int> counter(iParameters.size(), 0);
   for(int i = 0; i < iObs.size(); i++) {
      float obs = iObs[i].getValue();
      float fcst = iUnCorrected[i].getMoment(1);
      if(Global::isValid(obs) && Global::isValid(fcst) && fcst >= mEdges[0] && fcst <= mEdges[mEdges.size()-1]) {
         for(int i = 1; i < mEdges.size(); i++) {
            if(fcst < mEdges[i]) {
               int bin = i-1;
               totalObs[bin] += obs;
               counter[bin]++;
               break;
            }
         }
      }
   }
   for(int bin = 0; bin < iParameters.size(); bin++) {
      if(counter[bin] > 0) {
         float value = totalObs[bin]/counter[bin];
         iParameters[bin] = Processor::combine(iParameters[bin], value);
      }
   }
}
