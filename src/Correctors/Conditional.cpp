#include "Conditional.h"
#include "../Obs.h"
#include "../Parameters.h"

CorrectorConditional::CorrectorConditional(const Options& iOptions, const Data& iData) :
      Corrector(iOptions, iData) {
   //! Edges between correction bins
   iOptions.getRequiredValues("edges", mEdges);

   std::string interpolatorTag;
   //! How should the correction points be interpolated?
   if(iOptions.getValue("interpolator", interpolatorTag)) {
      mInterpolator = Interpolator::getScheme(interpolatorTag);
   }
   else {
      mInterpolator = Interpolator::getScheme(Options("class=InterpolatorLinear"));
   }

   for(int i = 1; i < mEdges.size(); i++) {
      mCenters.push_back((mEdges[i]+mEdges[i-1])/2);
   }

   iOptions.check();
}

void CorrectorConditional::correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const {
   // Correct each ensemble member
   for(int n = 0; n < iUnCorrected.size(); n++) {
      float fcst = iUnCorrected[n];
      if(Global::isValid(fcst)) {
         std::vector<float> param = iParameters.getAllParameters();
         int B = param.size()/2;
         std::vector<float> y(param.begin(), param.begin()+B); // mean observations
         std::vector<float> x(param.begin()+B, param.end());   // mean forecasts

         if(fcst >= x[0] and fcst <= x[B-1]) {
            iUnCorrected[n] = mInterpolator->interpolate(fcst, x, y);
         }
         else {
            // Don't correct forecasts outside the bins
         }
      }
   }
}

void CorrectorConditional::getDefaultParametersCore(Parameters& iParameters) const {
   // Store parameters as follows:
   // meanObs, ..., meanObs, meanFcst, ..., meanFcst
   std::vector<float> values;
   for(int k = 0; k < 2; k++) {
      for(int i = 0; i < mCenters.size(); i++) {
         values.push_back(mCenters[i]);
      }
   }
   iParameters.setAllParameters(values);
}

void CorrectorConditional::updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
   std::vector<float> param = iParameters.getAllParameters();
   int B = param.size()/2;

   std::vector<float> totalObs(B, 0);
   std::vector<float> totalFcst(B, 0);
   std::vector<int> counter(B, 0);
   for(int i = 0; i < iObs.size(); i++) {
      float obs = iObs[i].getValue();
      float fcst = iUnCorrected[i].getMoment(1);
      if(Global::isValid(obs) && Global::isValid(fcst) && fcst >= mEdges[0] && fcst <= mEdges[mEdges.size()-1]) {
         for(int i = 1; i < mEdges.size(); i++) {
            if(fcst < mEdges[i]) {
               int bin = i-1;
               totalObs[bin] += obs;
               totalFcst[bin] += fcst;
               counter[bin]++;
               break;
            }
         }
      }
   }
   for(int bin = 0; bin < B; bin++) {
      if(counter[bin] > 0) {
         float obsValue  = totalObs[bin]/counter[bin];
         float fcstValue = totalFcst[bin]/counter[bin];
         int obsI  = bin;
         int fcstI = bin+B;
         iParameters[obsI] = Processor::combine(iParameters[obsI], obsValue);
         iParameters[fcstI] = Processor::combine(iParameters[fcstI], fcstValue);
      }
   }
}
